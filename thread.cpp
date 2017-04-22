#include "thread.h"

void* send_thread_func(void* para)
{

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
  thread_send_para* para_send = (thread_send_para*)para;
  printf("It's me, thread %s!\n", (para_send->id).c_str());
  int sockfd = para_send->sockfd;

  /* purchase more */
  if(!send_APurchaseMore(whNum,  products, sockfd)){
    printf("send APurchaseMore failed\n");
  }
  /* pack */
  uint64_t shipId = 0;
  for(shipId = 1; shipId < 6; shipId++){
    if(!send_APack(whNum, shipId, pur, sockfd)){
      printf("send APack failed\n");
    }
  }
  
  pthread_exit(NULL);
}

void* recv_thread_func(void* para)
{
  thread_recv_para* para_recv = (thread_recv_para*)para;
  printf("It's me, thread %s!\n", (para_recv->id).c_str());
  AResponses res;
  while(true){
    recv_AResponse(para_recv->sockfd, res);
  }
  pthread_exit(NULL);
}
