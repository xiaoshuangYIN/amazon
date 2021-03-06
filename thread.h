#include <utility>  
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <functional> 
#include <unistd.h>
#include <pthread.h>
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

// send Acomds
struct _thread_send_para {
  std::string id;
  connection* C;
  int sockfd;
  int purchase_id;
  int worldid;
  int wh_count;
  std::vector<ACommands>* queue;
  google::protobuf::io::FileOutputStream * outfile;
};
typedef struct _thread_send_para thread_send_para;  


// recv Aresponse
struct _thread_recv_para {
  std::string id;
  int sockfd;
  connection* C;
  int wid;
  std::vector<ACommands>* queue;
  std::vector<AmazontoUPS>* Uqueue;
};
typedef struct _thread_recv_para thread_recv_para;  

// send Ucommands
struct _thread_pick_para {
  std::string id;
  int sockfd;// UPS conected to 
  connection* C;
};
typedef struct _thread_pick_para thread_pick_para;

// ship
struct _thread_ship_para {
  std::string id;
  int sockfd;
  std::vector<ACommands>* queue;
  std::vector<AmazontoUPS>* Uqueue;
  connection* C;
  int purchase_id;
  int worldid;
  int wh_count;
};
typedef struct _thread_ship_para thread_ship_para;  

// buy stock 
struct _thread_buy_para {
  int sockfd;
  connection* C;
  std::string id;
  int wh_count;
};
typedef struct _thread_buy_para thread_buy_para;  

// Ucom
struct _thread_Ucom_para {
  std::string id;
  google::protobuf::io::FileOutputStream * outfile;
  connection* C;
  int worldid;
  std::vector<AmazontoUPS>* queue;
};
typedef struct _thread_Ucom_para thread_Ucom_para;  

// Ures
struct _thread_Ures_para {
  std::string id;
  google::protobuf::io::FileInputStream * infile;
  connection* C;
  int worldid;
  int sockfd;
  std::vector<AmazontoUPS>* Uqueue;
  std::vector<ACommands>* queue;
};
typedef struct _thread_Ures_para thread_Ures_para;  



// funcs
void* ship_thread_func(void* para);
void* pack_thread_func(void* para);
void* load_thread_func(void* para);
void* send_thread_func(void* para);
void* recv_thread_func(void* para);
void* buy_thread_func(void* para);
void* UPS_recv_func(void* parav);
void* UPS_send_func(void* parav);
