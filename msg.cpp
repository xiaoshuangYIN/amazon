#include "msg.h"
template <class T>
void to_String(T a, std::string& res){
  std::stringstream ss;
  ss << a;
  res = ss.str();
  ss.str("");
}

bool sendUMesgTo(AmazontoUPS &message , google::protobuf::io::FileOutputStream * out){
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

bool recvUMesgFrom(UPStoAmazon& message,
		   google::protobuf::io::FileInputStream* in ){
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

bool recv_UConnected(int worldid,   google::protobuf::io::FileInputStream * infile){
  UPStoAmazon Ures;
  if(!  recvUMesgFrom(Ures, infile)){
    return false;
  }
  return true;
}

bool send_UConnected(int worldid,   google::protobuf::io::FileOutputStream * outfile){
  AmazontoUPS Ucom;
  AConnectedToSim* conn = Ucom.add_aconnected();
  conn->set_worldid(worldid);
  
  if(!  sendUMesgTo(Ucom, outfile)){
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
  //std::cout<<comd.DebugString()<<"\n";

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
  //std::cout<<comd.DebugString()<<"\n";

  /* send ACommand(APack) */
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  if ( sendMesgTo(comd, outfile) == false){
    std::cout<<"amazon server: APack fail to send\n";
    return false;
  }
  return true;
}

// TO DO

void parse_AResponses(AResponses Ares, std::vector<int>& readys, std::vector<int>& loadeds, std::vector<std::pair<int, std::vector<std::unordered_map<std::string, std::string> > > >& arriveds){

  if(Ares.arrived_size() > 0){
    for(int i = 0; i < Ares.arrived_size(); i++){
      std::pair<int, std::vector<std::unordered_map<std::string, std::string> > > wh_prods;

      std::vector<std::unordered_map<std::string, std::string> >prods;
      std::unordered_map<std::string, std::string> prod;
      for(int j = 0; j < (Ares.arrived(i)).things_size(); j++){
	

	std::string id;
	to_String(Ares.arrived(i).things(j).id(), id);
	prod["id"]= id;

	std::string desc;
	to_String(Ares.arrived(i).things(j).description(), desc);
	prod["description"]= desc;

	std::string count;
	to_String(Ares.arrived(i).things(j).count(), count);
	prod["count"]= count;
	
	prods.push_back(prod);
      }//for
      wh_prods = std::make_pair(Ares.arrived(i).whnum(), prods);
      arriveds.push_back(wh_prods);
    }// for
  }// if
  // ready
  if(Ares.ready_size() > 0){
    for(int i = 0; i < Ares.ready_size(); i++){
      readys.push_back(Ares.ready(i));
    }
  }
  // loaded
  if(Ares.loaded_size() > 0){
    for(int i = 0; i < Ares.loaded_size(); i++){
      readys.push_back(Ares.loaded(i));
    }
  }
  //error

}



bool recv_parse_AResponse(int sockfd,  std::vector<int>& readys, std::vector<int>& loadeds, std::vector<std::pair<int, std::vector<std::unordered_map<std::string, std::string> > > >& arriveds){
  // receive
  AResponses res;
  google::protobuf::io::FileInputStream * infile = new google::protobuf::io::FileInputStream(sockfd);
  if (!recvMesgFrom(res, infile)){
    std::cerr<<"amazon server: AResponse fail to recv\n";
    return false;
  }
  // print response
  printf("rec from sim: %s\n", res.DebugString().c_str());

  // parse
  parse_AResponses(res, readys, loadeds, arriveds);

  // return
  return true;
}

void create_ACom_APack(uint32_t whNum, uint64_t shipId, std::vector<std::unordered_map<std::string, std::string> > &products,   ACommands& comd){
  /* Acommands(APack) */

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
  std::cout<<"create apack\n"<<std::endl;
  std::cout<<comd.DebugString()<<"\n";

}

void create_ACom_ALoad(std::unordered_map<std::string, int> & load_m,   ACommands& comd){

  /* Acommands(APack) */

    APutOnTruck* load = comd.add_load();
    //    int32_t whNum;
    //int32_t truckId;
    //int64_t shipId;
    //std::stringstream((load_m)["whNum"]) >> whNum;
    //std::stringstream((load_m)["shipId"]) >> shipId;
    //std::stringstream((load_m)["truckId"]) >> truckId;
    //load->set_whnum(whNum);
    //load->set_shipid(shipId);
    //load->set_truckid(truckId);
    load->set_whnum(load_m["wid"]);
    load->set_shipid(load_m["sid"]);
    load->set_truckid(load_m["truckid"]);
    std::cout<< comd.DebugString() <<"\n";

}

void create_UCom_UPick(std::unordered_map<std::string, int>  & package, int whid, AmazontoUPS& comd){
    sendTruck* pickup =  comd.add_send_truck();
    pickup->set_whid((uint32_t)whid);
    
    pkgInfo* pkg = pickup->add_packages();
    pkg->set_packageid((uint32_t)package["sid"]);
    pkg->set_delx((uint32_t)package["delx"]);
    pkg->set_dely((uint32_t)package["dely"]);
    //package->set_upsAccount((vv_packages[i][j])["upsact"]);
}



void create_UCom_UDispatch(std::unordered_map<std::string, int> & package, int sid, AmazontoUPS& comd){
    dispatchTruck* dispatch =  comd.add_dispatch_truck();
    dispatch->set_truckid(package["truckid"]);
    
    pkgInfo* pkg = dispatch->add_packages();
    pkg->set_packageid((uint32_t)package["sid"]);
    pkg->set_delx((uint32_t)package["delx"]);
    pkg->set_dely((uint32_t)package["dely"]);
    //package->set_upsAccount((vv_packages[i][j])["upsact"]);
}

void parse_UResponses(UPStoAmazon msg, std::vector<std::unordered_map<std::string, int> >& truck_arriveds, std::vector<std::unordered_map<std::string, int> >& delivereds){
  if(msg.truck_arrived_size() > 0){
    for(int i = 0; i < msg.truck_arrived_size(); i++){
      std::unordered_map<std::string, int> map;
      map["whid"] = msg.truck_arrived(i).whid();
      map["truckid"] = msg.truck_arrived(i).truckid();
      truck_arriveds.push_back(map);
    }
  }
  if(msg.delivered_size() > 0){
    for(int i = 0; i < msg.delivered_size(); i++){
      std::unordered_map<std::string, int> map;
      map["packageid"] = msg.delivered(i).packageid();
      delivereds.push_back(map);
    }
  }

}


bool recv_parse_UResponse(google::protobuf::io::FileInputStream * in , std::vector<std::unordered_map<std::string, int> >& truck_arriveds, std::vector<std::unordered_map<std::string, int> >& delivereds){
  UPStoAmazon msg;
  if (! recvUMesgFrom(msg, in)){
    std::cerr<<"amazon server: UResponse fail to recv\n";
    return false;
  }
  // print response
  printf("rec from UPS: %s\n", msg.DebugString().c_str());

  // parse
  parse_UResponses(msg, truck_arriveds, delivereds);

  // return
  return true;
  
}
