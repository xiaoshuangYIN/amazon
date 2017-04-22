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

  /*  connect socket to sim */

  int sockfd, numbytes;  
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  set_hints(&hints);
  get_addr_info(&hints, &servinfo, &rv, PORT, sim_IP);  
  connect_sock(&servinfo, sockfd,  s);

  /* Aconnect and Aconnected with sim*/

  uint64_t worldid = 1049;// recv from UPS
  if(!send_AConnect_recv_AConnected(worldid, sockfd)){
    printf("send AConnect receive AConnected failed\n");
  }

  /* speed up */
  /*
  uint32_t speed = 6000;
  if(!send_simspeed(speed, sockfd)){
    printf("speed up fail\n");
  }
  */

  
  /* create two thrads*/
  pthread_t thread_send;
  pthread_t thread_recv;
  int rc1, rc2;
  thread_send_para send_para;
  send_para.id = std::string("send");
  send_para.sockfd = sockfd;
  thread_recv_para recv_para;
  recv_para.id = std::string("recv");
  recv_para.sockfd = sockfd;

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


  /* Last thing that main() should do */
  pthread_exit(NULL);
  return 0;
}


