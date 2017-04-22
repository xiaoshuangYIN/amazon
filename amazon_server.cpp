#include "server_base.h"
#include "msg.h"

#include <string>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>

const char* sim_IP = "10.236.48.17";
const char* PORT = "23456";


int main(int argc, char* argv[]){

  /******* fetch from db ******/
  uint32_t whNum = 1;
  std::vector<std::unordered_map<std::string, std::string> > products;
  std::unordered_map<std::string, std::string> map1;
  map1["id"] = "10";
  map1["description"]="cake";
  map1["count"]="5";
  std::unordered_map<std::string, std::string> map2;
  map2["id"] = "13";
  map2["description"]="candy";
  map2["count"]="5";
  products.push_back(map1);
  products.push_back(map2);
  
  std::vector<std::unordered_map<std::string, std::string> > pur;
  std::unordered_map<std::string, std::string> m1;
  m1["id"] = "10";
  m1["description"]="cake";
  m1["count"]="1";
  std::unordered_map<std::string, std::string> m2;
  m2["id"] = "13";
  m2["description"]="candy";
  m2["count"]="1";
  pur.push_back(m1);
  pur.push_back(m2);
  
  /* *********************** */
  
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
  
  
  /* AConnect */
  uint64_t worldid = 1048;// recv from UPS
  if(!send_AConnect(worldid, sockfd)){
    printf("send AConnect receive AConnected failed\n");
  }
  /* spped up */
  uint32_t speed = 60000;
  if(!send_simspeed(speed, sockfd)){
    printf("speed up fail\n");
  }
  /* a thread keep receive msg from sim */

  /* multi thread keep send msg to sim  */
  /* ACommands */
  // purchase more
  if(!send_APurchaseMore(whNum,  products, sockfd)){
    printf("send APurchaseMore failed\n");
  }
  // pack
  uint64_t shipId = 0;
  for(shipId = 1; shipId < 6; shipId++){
    if(!send_APack(whNum, shipId, pur, sockfd)){
      printf("send APack failed\n");
    }
  }
  /*Delete all global objects allocated by libprotobuf */
  google::protobuf::ShutdownProtobufLibrary();
  
  return 0;
}
