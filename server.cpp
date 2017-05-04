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
//const char* sim_IP = "10.233.48.5";
const char* PORT = "23456";
const char* au_PORT =  "34567";

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
  uint32_t speed = 10000000;
  if(!send_simspeed(speed, sockfd)){
    printf("speed up fail\n");
  }


  // accept connection from UPS
  int sockfd_server, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints_server;
  struct addrinfo* servinfo_server;
  struct sockaddr_storage their_addr_server; // connector's address information
  socklen_t sin_size_server;
  struct sigaction sa_server;
  int yes_server=1;
  char s_server[INET6_ADDRSTRLEN];
  int rv_server;  

  set_socket(&hints_server);
  get_addr_info_server(&hints_server, &servinfo_server, &rv_server, au_PORT);
  bind_sock(&servinfo_server, &sockfd_server, &yes_server);
  listen_sock(&sockfd_server);
  sigaction(&sa_server);
  printf("server: waiting for connections from UPS...\n");            

  new_fd = accept_sock(sockfd_server, their_addr_server);


  while(new_fd == -1){
    new_fd = accept_sock(sockfd_server, their_addr_server);
  }

  /* init filestream , send Uconnected through new_fd */
  google::protobuf::io::FileOutputStream * UPS_out = new google::protobuf::io::FileOutputStream(new_fd);
  google::protobuf::io::FileInputStream * UPS_in = new google::protobuf::io::FileInputStream(new_fd);
  
  /*
  if(!recv_UConnected(worldid, UPS_in)){
    printf("receive UConnected failed\n");
  }
  */

  
    
  if(!send_UConnected(worldid, UPS_out)){
    printf("send UConnected failed\n");
  }


  


  
  /* init two queues*/
  std::vector<ACommands>* ACom_queue = new std::vector<ACommands>();
  std::vector<AmazontoUPS>* UCom_queue = new std::vector<AmazontoUPS>();

  
  /* create two thrads */
  pthread_t thread_send;
  pthread_t thread_recv;
  pthread_t thread_buy;
  pthread_t thread_ship;
  pthread_t thread_UPS_send;
  pthread_t thread_UPS_recv;
  int rc1, rc2, rc3, rc4, rc5, rc6;

  thread_ship_para ship_para;
  ship_para.id = std::string("ship");
  ship_para.queue = ACom_queue;
  ship_para.Uqueue = UCom_queue;
  ship_para.C = C;
  ship_para.purchase_id = purchase_id;
  ship_para.wh_count = wh_count;
  ship_para.worldid = worldid;
  
  thread_send_para send_para;
  send_para.id = std::string("send");
  send_para.queue = ACom_queue;
  send_para.sockfd = sockfd;//op
  google::protobuf::io::FileOutputStream * outfile = new google::protobuf::io::FileOutputStream(sockfd);
  send_para.outfile = outfile;
  
  thread_recv_para recv_para;
  recv_para.id = std::string("recv");
  recv_para.sockfd = sockfd;
  recv_para.C = C;
  recv_para.wid = worldid;
  recv_para.queue = ACom_queue;
  recv_para.Uqueue = UCom_queue;
  
  thread_buy_para buy_para;
  buy_para.id = std::string("buy");
  buy_para.sockfd = sockfd;
  buy_para.C = C;
  buy_para.wh_count = wh_count;


  thread_Ucom_para Ucom_para;
  Ucom_para.id = std::string("send_Ucommands");
  Ucom_para.outfile = UPS_out;
  Ucom_para.C = C;
  Ucom_para.worldid = worldid;
  Ucom_para.queue = UCom_queue;

  thread_Ures_para Ures_para;
  Ures_para.id = std::string("recv_Uresponses");
  Ures_para.C = C;
  Ures_para.worldid = worldid;
  Ures_para.sockfd = sockfd_server;
  Ures_para.infile = UPS_in;
  Ures_para.queue = ACom_queue;
  Ures_para.Uqueue = UCom_queue;
  
  rc4 = pthread_create(&thread_ship, NULL, ship_thread_func, &ship_para);
  if (rc4){
    printf("ERROR; return code from pthread_create() is %d\n", rc4);
    exit(-1);
  }
  
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
  

  /*
  rc3 = pthread_create(&thread_buy, NULL, buy_thread_func, &buy_para);
  if (rc3){
    printf("ERROR; return code from pthread_create() is %d\n", rc3);
    exit(-1);
  }
  */

  rc5 = pthread_create(&thread_UPS_recv, NULL, UPS_recv_func, &Ures_para);
  if (rc5){
    printf("ERROR; return code from pthread_create() is %d\n", rc5);
    exit(-1);
  }

  rc6 = pthread_create(&thread_UPS_send, NULL, UPS_send_func, &Ucom_para);
  if (rc6){
    printf("ERROR; return code from pthread_create() is %d\n", rc6);
    exit(-1);
  }


  /* Last thing that main() should do */
  pthread_exit(NULL);
  delete ACom_queue;

  return 0;
}


