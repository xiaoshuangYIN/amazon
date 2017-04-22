#include "server_base.h"

void set_hints(struct addrinfo* hints_p){
  memset(hints_p, 0, sizeof (*hints_p));
  (*hints_p).ai_family = AF_UNSPEC;
  (*hints_p).ai_socktype = SOCK_STREAM;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void get_addr_info(struct addrinfo* hints_p, struct addrinfo** servinfo, int* rv, const char* PORT, const char* sim_IP){
  if ((*rv = getaddrinfo(sim_IP, PORT, hints_p, servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(*rv));
    exit(1);
  }
}

void connect_sock(struct addrinfo** servinfo, int& sockfd, char* s){
  struct addrinfo* p;

  for(p = *servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      perror("amazon server: socket");
      continue;
    }
    
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      sockfd = -1;
      perror("amazon server: connect");
      continue;
    }
    break;
  }
  if (p == NULL) {
    fprintf(stderr, "amazon server: failed to connect\n");
    exit(1);
  }
  
  printf("connect success\n");
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
  freeaddrinfo(*servinfo); // all done with this structure
}
