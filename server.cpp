#include "server_base.h"
#include "protocal/amazon.pb.h"
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
    std::cout<<"read size false\n";
    return false;
  }
  printf("receive size : %d\n", size);
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


  /*  connect socket to sim */
  int sockfd, numbytes;  
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  set_hints(&hints);
  get_addr_info(&hints, &servinfo, &rv, PORT, sim_IP);  
  connect_sock(&servinfo, &sockfd,  s);

  /* google protoco buffer */
  /* write connect message */

  AConnect* conn = new AConnect();
  conn->set_worldid(1);

  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  bool suc = sendMesgTo(*conn, outfile);
  if (suc == false){
    std::cout<<"amazon server: Aconnect fail to send\n";
  }

  /* receive */
  AConnected* connRep = new AConnected();
  google::protobuf::io::FileInputStream * infile = new google::protobuf::io::FileInputStream(sockfd);
  suc = recvMesgFrom(*connRep, infile);
  if (suc == false){
    std::cout<<"amazon server: Aconnected fail to recv\n";
  }

  
  /* test */
  printf("response: %s\n",connRep->error().c_str());
  

  
  /*Delete all global objects allocated by libprotobuf */
  google::protobuf::ShutdownProtobufLibrary();
  
  return 0;
}
