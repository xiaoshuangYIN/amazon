#include "protocal/amazon_orig_3.pb.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <stdint.h>

/* send message and receive is seperated */
template<typename T> bool sendMesgTo(const T & , google::protobuf::io::FileOutputStream *);
template<typename T> bool recvMesgFrom(T & , google::protobuf::io::FileInputStream * );
bool send_AConnect(uint64_t , int);
bool send_APack(uint32_t , uint64_t , std::vector<std::unordered_map<std::string, std::string> >& , int );
bool send_APurchaseMore(uint32_t , std::vector<std::unordered_map<std::string, std::string> > &, int );
bool send_simspeed(uint32_t speed, int sockfd);
bool send_AConnect_recv_AConnected(uint64_t worldid, int sockfd);
