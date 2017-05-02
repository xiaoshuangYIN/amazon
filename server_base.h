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

#define BACKLOG 10     // how many pending connections queue will hold



void set_hints(struct addrinfo* hints_p);
void *get_in_addr(struct sockaddr *sa);
void get_addr_info(struct addrinfo* hints_p, struct addrinfo** servinfo, int* rv, const char* sim_IP, const char* PORT);
void connect_sock(struct addrinfo** servinfo, int& sockfd_p, char* s);

void change_nonblock(int sockfd);

void set_socket(addrinfo* hints_po);
void bind_sock(struct addrinfo** servinfo, int* sockfd, int* yes);
void sigchld_handler(int s);
void sigaction(struct sigaction* sa);
void listen_sock(int* sockfd);
void get_addr_info_server(struct addrinfo* hints, struct addrinfo** servinfo, int* rv, const char* AU_PORT);
void close_sock(int* sockfd);
int accept_sock(int sockfd, struct sockaddr_storage their_addr);
