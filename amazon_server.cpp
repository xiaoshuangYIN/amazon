#include "server_base.h"
//#include "GPB.h"
#include "protocal/amazon_orig_2.pb.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <string>
#include <iostream>
#include <string>
#include <unordered_map>

const char* sim_IP = "10.236.48.17";
const char* PORT = "23456";
template<typename T> bool sendMesgTo(const T & message, google::protobuf::io::FileOutputStream *out) {
  {
    //extra scope: make output go away before out->Flush()
    // We create a new coded stream for each message.  Don't worry, this is fast.
    google::protobuf::io::CodedOutputStream output(out);
    // Write the size.
    const int size = message.ByteSize();
    output.WriteVarint32(size);
    uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
    if (buffer != NULL) {
      // Optimization:  The message fits in one buffer, so use the faster
      // direct-to-array serialization path.
      message.SerializeWithCachedSizesToArray(buffer);
    } else {
      // Slightly-slower path when the message is multiple buffers.
      message.SerializeWithCachedSizes(&output);
      if (output.HadError()) {
	return false;
      }
    }
  }

  //std::cout << "flushing...\n";
  out->Flush();
  return true;
}

template<typename T> bool recvMesgFrom(T & message,
				       google::protobuf::io::FileInputStream * in ){
  // We create a new coded stream for each message.  Don't worry, this is fast,
  // and it makes sure the 64MB total size limit is imposed per-message rather
  // than on the whole stream.  (See the CodedInputStream interface for more
  // info on this limit.)
  google::protobuf::io::CodedInputStream input(in);
  // Read the size.
  uint32_t size;
  if (!input.ReadVarint32(&size)) {
    return false;
  }
  // Tell the stream not to read beyond that size.
  google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);
  // Parse the message.
  if (!message.MergeFromCodedStream(&input)) {
    return false;
  }
  if (!input.ConsumedEntireMessage()) {
    return false;
  }
  // Release the limit.
  input.PopLimit(limit);
  return true;
}

int main(int argc, char* argv[]){

  /*  create database conncection */
  // connection* C = create_connection();
  //create_tables(C);

  /*  connect socket to sim */
  int sockfd, numbytes;  
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  set_hints(&hints);
  get_addr_info(&hints, &servinfo, &rv, PORT, sim_IP);  
  connect_sock(&servinfo, sockfd,  s);

  /* google protoco buffer */
  /* AConnect */
  AConnect conn;
  conn.set_worldid(1043);// worldid = the value after init world

  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if ( sendMesgTo(conn, outfile) == false){
    std::cout<<"amazon server: Aconnect fail to send\n";
  }

  /* receive */
  AConnected connRes;
  google::protobuf::io::FileInputStream * infile = new google::protobuf::io::FileInputStream(sockfd);
  if (recvMesgFrom(connRes, infile)== false){
    std::cout<<"amazon server: Aconnected fail to recv\n";
  }


  /* APack */
  /* fetch from db */
  int whNum = 1;
  int shipId = 1;
  std::vector<std::unordered_map<std::string, std::string> > products;
  /* ****** */
  APack pack;
  pack.set_whnum(whNum);
  pack.set_shipid(shipId);
  AProduct* prod;
  for(int i = 0; i < products.size(); i++){
    prod = pack.add_things();
    prod->set_id((std::stoi((products[i])["id"]), NULL));
    prod->set_description((products[i])["description"]);
    prod->set_count((std::stoi((products[i])["count"]), NULL));
  }
  
  /* test */
  if (connRes.has_error() ){ 
    printf("response: %s\n",connRes.error().c_str());
  }
  else {
    printf("Connection was ok\n");
  }

  
  /*Delete all global objects allocated by libprotobuf */
  google::protobuf::ShutdownProtobufLibrary();
  
  return 0;
}
