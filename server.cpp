#include "server_base.h"
#include "thread.h"
#include "msg.h"


#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <pthread.h>
#include <stdio.h>


const char* sim_IP = "10.236.48.17";
const char* PORT = "23456";


int main(int argc, char* argv[]){
  if(argc < 4){
    std::cerr<<"usage: ./server <purchase_id to start> <worldid> <num_wh>\n";
    exit(1);
  }
  int purchase_id = atoi(argv[1]);
  int worldid = atoi(argv[2]);
  int wh_count = atoi(argv[3]);

  
  /* connect to database */
  connection* C = create_connection();

  /*  connect socket to sim */
  int sockfd, numbytes;  
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  set_hints(&hints);
  get_addr_info(&hints, &servinfo, &rv, PORT, sim_IP);  
  connect_sock(&servinfo, sockfd,  s);


  /* Aconnect and Aconnected with sim*/
   if(!send_AConnect_recv_AConnected(worldid, sockfd)){
    printf("send AConnect receive AConnected failed\n");
  }

  /* speed up */
  
  uint32_t speed = 60000;
  if(!send_simspeed(speed, sockfd)){
    printf("speed up fail\n");
  }
  
    
  /* create two thrads */
  pthread_t thread_send;
  pthread_t thread_recv;
  pthread_t thread_buy;
  int rc1, rc2, rc3;
  thread_send_para send_para;
  send_para.id = std::string("send");
  send_para.sockfd = sockfd;
  send_para.C = C;
  send_para.purchase_id = purchase_id;
  send_para.wh_count = wh_count;
  send_para.worldid = worldid;
  thread_recv_para recv_para;
  recv_para.id = std::string("recv");
  recv_para.sockfd = sockfd;
  recv_para.C = C;
  thread_buy_para buy_para;
  buy_para.id = std::string("buy");
  buy_para.sockfd = sockfd;
  buy_para.C = C;
  buy_para.wh_count = wh_count;
  rc1 = pthread_create(&thread_send, NULL, send_thread_func, &send_para);
  if (rc1){
    printf("ERROR; return code from pthread_create() is %d\n", rc1);
    exit(-1);
  }

  rc2 = pthread_create(&thread_recv, NULL, recv_thread_func, &recv_para);
  if (rc2){
    printf("ERROR; return code from pthread_create() is %d\n", rc2);
    exit(-1);
  }
  
  rc3 = pthread_create(&thread_buy, NULL, buy_thread_func, &buy_para);
  if (rc3){
    printf("ERROR; return code from pthread_create() is %d\n", rc3);
    exit(-1);
  }
 

  /* Last thing that main() should do */
  pthread_exit(NULL);
  return 0;
}


