#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <functional> 
#include <unistd.h>
#include "msg.h"
#include "psql.h"

class comp {
  bool reverse;
public:
  comp(const bool& revparam=false)
  {reverse=revparam;}
  bool operator() (std::unordered_map<std::string, int>& lhs,
		   std::unordered_map<std::string, int>&rhs) const
  {
    if (reverse) return ((lhs["num"]) > (rhs["num"]));
    else return ((lhs["num"]) < (rhs["num"]));
  }
};

typedef std::priority_queue<std::unordered_map<std::string, int>,
  std::vector<std::unordered_map<std::string, int> >,
  comp> pq_t;
//
struct _thread_send_para {
  std::string id;
  int sockfd;
  connection* C;
  int purchase_id;
  int worldid;
  int wh_count;
};
typedef struct _thread_send_para thread_send_para;  
//
struct _thread_recv_para {
  std::string id;
  int sockfd;
  connection* C;
};
typedef struct _thread_recv_para thread_recv_para;  
//
struct _thread_pick_para {
  std::string id;
  int sockfd;// UPS conected to 
  connection* C;
};
typedef struct _thread_pick_para thread_pick_para;
//
struct _thread_pack_para {
  std::string id;
  connection* C;
  int sockfd;
};
typedef struct _thread_pack_para thread_pack_para;
//
struct _thread_load_para {
  std::string id;
  connection* C;
  int sockfd;
};
typedef struct _thread_load_para thread_load_para;
//
struct _thread_buy_para {
  int sockfd;
  connection* C;
  std::string id;
  int wh_count;
};
typedef struct _thread_buy_para thread_buy_para;  
//
void* pack_thread_func(void* para);
void* load_thread_func(void* para);
void* send_thread_func(void* para);
void* recv_thread_func(void* para);
void* buy_thread_func(void* para);
