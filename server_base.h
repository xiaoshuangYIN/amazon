#include <stdlib.h>
#include <unistd.h>

#include <inttypes.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>


#include <vector>
#include <string>
#include <unordered_map>



void set_hints(struct addrinfo* hints_p);
void *get_in_addr(struct sockaddr *sa);
void get_addr_info(struct addrinfo* hints_p, struct addrinfo** servinfo, int* rv, const char* sim_IP, const char* PORT);
void connect_sock(struct addrinfo** servinfo, int& sockfd_p, char* s);
