#include "msg.h"



template<typename T> bool sendMesgTo(const T & message, google::protobuf::io::FileOutputStream *out) {
  {

    google::protobuf::io::CodedOutputStream output(out);
    // Write the size.
    const int size = message.ByteSize();
    output.WriteVarint32(size);
    uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
    if (buffer != NULL) {

      message.SerializeWithCachedSizesToArray(buffer);
    }

    else {
      
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

void recv_AResponse(int sockfd, AResponses& res){
  /* receive */
  google::protobuf::io::FileInputStream * infile = new google::protobuf::io::FileInputStream(sockfd);
  if (!recvMesgFrom(res, infile)){
    std::cerr<<"amazon server: AResponse fail to recv\n";
  }
  /* test AConnected */
  printf("rec from sim: %s\n", res.DebugString().c_str());
}

bool send_AConnect(uint64_t worldid, int sockfd){
  AConnect conn;
  conn.set_worldid(worldid);// worldid = the value after init world
  /* send AConnect*/
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if (! sendMesgTo(conn, outfile)){
    std::cout<<"amazon server: Aconnect fail to send\n";
    return false;
  }
  
  return true;
}
bool send_AConnect_recv_AConnected(uint64_t worldid, int sockfd){
  AConnect conn;
  conn.set_worldid(worldid);// worldid = the value after init world

  /* send AConnect*/
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if (! sendMesgTo(conn, outfile)){
    std::cout<<"amazon server: Aconnect fail to send\n";
    return false;
  }

  /* receive */
  AConnected connRes;

  google::protobuf::io::FileInputStream * infile = new google::protobuf::io::FileInputStream(sockfd);
  if (!recvMesgFrom(connRes, infile)){
    std::cout<<"amazon server: Aconnected fail to recv\n";
    return false;
  }
  /* test AConnected */
  if (connRes.error() != ""){
    printf("response: %s\n",connRes.error().c_str());
  }
  else {
    printf("Connection error is empty\n");
  }
  return true;
}


bool send_APack(uint32_t whNum, uint64_t shipId, std::vector<std::unordered_map<std::string, std::string> > &products, int sockfd){
  /* Acommands(APack) */

  ACommands comd;
  APack* pack = comd.add_topack();
  pack->set_whnum(whNum);
  pack->set_shipid(shipId);

  AProduct* prod;
  uint64_t id = 0;
  uint32_t count = 0;
  for(int i = 0; i < products.size(); i++){
    prod = pack->add_things();
    std::stringstream((products[i])["pid"]) >> id;
    std::stringstream((products[i])["count"]) >> count;
    prod->set_description((products[i])["desc"]);
    prod->set_count(count);
    prod->set_id(id);
  }
  std::cout<<comd.DebugString()<<"\n";

  /* send ACommand(APack) */
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if ( sendMesgTo(comd, outfile) == false){
    std::cout<<"amazon server: APack fail to send\n";
    return false;
  }
  return true;
}

bool send_APurchaseMore(uint32_t whNum, std::vector<std::unordered_map<std::string, std::string> > & products, int sockfd){
  /* Acommands(APurchaseMore) */
  ACommands comd;
  APurchaseMore* purchase = comd.add_buy();
  purchase->set_whnum(whNum);

  AProduct* prod;
  uint64_t id = 0;
  uint32_t count = 0;
  for(int i = 0; i < products.size(); i++){
    prod = purchase->add_things();
    std::stringstream((products[i])["id"]) >> id;
    std::stringstream((products[i])["count"]) >> count;
    prod->set_description((products[i])["description"]);
    prod->set_count(count);
    prod->set_id(id);
  }
  std::cout<<comd.DebugString()<<"\n";

  /* send ACommand(APack) */
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if ( sendMesgTo(comd, outfile) == false){
    std::cout<<"amazon server: APack fail to send\n";
    return false;
  }
  return true;
}

//optional uint32 simspeed = 4; //units/minute

bool send_simspeed(uint32_t speed, int sockfd){
  /* Acommands(APurchaseMore) */
  ACommands comd;
  comd.set_simspeed(speed);

  /* send ACommand(APack) */
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if ( sendMesgTo(comd, outfile) == false){
    std::cout<<"amazon server: APack fail to send\n";
    return false;
  }
  return true;
}
bool send_APutOnTruck(int sockfd, std::vector<std::unordered_map<std::string, std::string> > loads){


  /* Acommands(APack) */

  ACommands comd;
  for (int i = 0; i < loads.size(); i++){
    APutOnTruck* load = comd.add_load();
    int32_t whNum;
    int32_t truckId;
    int64_t shipId;
    std::stringstream((loads[i])["whNum"]) >> whNum;
    std::stringstream((loads[i])["shipId"]) >> shipId;
    std::stringstream((loads[i])["truckId"]) >> truckId;
    load->set_whnum(whNum);
    load->set_shipid(shipId);
    load->set_truckid(truckId);
  }
  std::cout<<comd.DebugString()<<"\n";

  /* send ACommand(APack) */
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if ( sendMesgTo(comd, outfile) == false){
    std::cout<<"amazon server: APack fail to send\n";
    return false;
  }
  return true;
}
