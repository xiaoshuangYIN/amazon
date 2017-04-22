#include <string>
#include <stdio.h>
#include <stdlib.h>

struct _thread_send_para {
  std::string id;
};
typedef struct _thread_send_para thread_send_para;  

struct _thread_recv_para {
  std::string id;
};
typedef struct _thread_recv_para thread_recv_para;
void* send_thread_func(void* para);
void* recv_thread_func(void* para);
