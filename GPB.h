#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

//#include "protocal/amazon_orig_2.pb.h"

#include <iostream>

template<typename T> bool sendMesgTo(const T & , google::protobuf::io::FileOutputStream *);
template<typename T> bool recvMesgFrom(T & , google::protobuf::io::FileInputStream * );
//std::ostream & operator<<(std::ostream & s, const google::protobuf::Message & m);
