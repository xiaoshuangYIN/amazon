#include "thread.h"

void* send_thread_func(void* para)
{
  thread_send_para* para_send = (thread_send_para*)para;
  printf("It's me, thread %s!\n", (para_send->id).c_str());
  pthread_exit(NULL);
}

void* recv_thread_func(void* para)
{
  thread_recv_para* para_recv = (thread_recv_para*)para;
  printf("It's me, thread %s!\n", (para_recv->id).c_str());
  pthread_exit(NULL);
}
