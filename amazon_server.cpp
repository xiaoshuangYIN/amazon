#include "server_base.h"
#include "GPB.h"
#include "protocal/amazon_orig_2.pb.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <string>
#include <iostream>

const char* sim_IP = "10.236.48.17";
const char* PORT = "23456";

template<typename T> bool sendMesgTo(const T & message, google::protobuf::io::FileOutputStream *out) {
  {
    //extra scope: make output go away before out->Flush()
    google::protobuf::io::CodedOutputStream output(out);
    const int size = message.ByteSize();
    output.WriteVarint32(size);
    uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
    if (buffer != NULL) {
      message.SerializeWithCachedSizesToArray(buffer);
    } else {
      message.SerializeWithCachedSizes(&output);
      if (output.HadError()) {
	return false;
      }
    }
  }
  out->Flush();
  return true;
}

template<typename T> bool recvMesgFrom(T & message,
				       google::protobuf::io::FileInputStream * in ){

  google::protobuf::io::CodedInputStream input(in);

  uint32_t size;
  if (!input.ReadVarint32(&size)) {
    std::cout<<"read size false, exit......\n";
    return false;
  }
  printf("receive size : %d\n", size);
  google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);
  if (!message.MergeFromCodedStream(&input)) {
    return false;
  }
  if (!input.ConsumedEntireMessage()) {
    return false;
  }
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
  /* write connect message */
  AConnect conn;
  conn.set_worldid(1043);
  //AInitWarehouse* initWh = conn.add_initwh();
  //initWh->set_x(-664);
  //initWh->set_y(-1081);
 

  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  bool suc = sendMesgTo(conn, outfile);
  if (suc == false){
    std::cout<<"amazon server: Aconnect fail to send\n";
  }

  /* receive */
  AConnected connRes;
  google::protobuf::io::FileInputStream * infile = new google::protobuf::io::FileInputStream(sockfd);
  suc = recvMesgFrom(connRes, infile);
  if (suc == false){
    std::cout<<"amazon server: Aconnected fail to recv\n";
  }

  
  /* test */
  //AConnect connTest;
  //connTest.ParseFromString(*output);
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
