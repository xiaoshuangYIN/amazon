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
    /*
    int status = fcntl(sockfd, F_SETFL, O_NONBLOCK);
    if (status == -1){
      perror("calling fcntl");
    }
    */
    //fcntl(s, F_SETFL, O_ASYNC);     // set to asynchronous I/O
    
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


void change_nonblock(int sockfd){
  // change to non-blocking after connected
  int status = fcntl(sockfd, F_SETFL, O_NONBLOCK);
  if (status == -1){
      perror("calling fcntl");
  }
    
  //fcntl(s, F_SETFL, O_ASYNC);     // set to asynchronous I/O
  
}


void set_socket(addrinfo* hints_p){
  memset(hints_p, 0, sizeof(*hints_p));
  hints_p->ai_family = AF_UNSPEC;
  hints_p->ai_socktype = SOCK_STREAM;
  hints_p->ai_flags = AI_PASSIVE; // use my IP
  return;
}

// get sockaddr, IPv4 or IPv6:
/*
void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
*/

void get_addr_info_server(struct addrinfo* hints_p, struct addrinfo** servinfo, int* rv, const char* AU_PORT){

  if ((*rv = getaddrinfo(NULL, AU_PORT, hints_p, servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(*rv));
    exit(1);
  }
}

void bind_sock(struct addrinfo** servinfo, int* sockfd_p, int* yes_p){
  // loop through all the results and bind to the first we can
  struct addrinfo* p;
  int fd;
  for(p = *servinfo; p != NULL; p = p->ai_next) {
    if ((fd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes_p,
		   sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    
    if (bind(fd, p->ai_addr, p->ai_addrlen) == -1){
      close(fd);
      perror("server: bind");
      continue;
    }
    break;
  }
  
  freeaddrinfo(*servinfo); // all done with this structure

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  *sockfd_p = fd;
  return;
}

int accept_sock(int sockfd, struct sockaddr_storage their_addr){
  socklen_t sin_size = sizeof (their_addr);
   int new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
  if (new_fd == -1) {
    perror("accept");
  }
  return new_fd; 
}

void sigchld_handler(int s){
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;
  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}

void sigaction(struct sigaction* sa_p){
  sa_p->sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa_p->sa_mask);
  sa_p->sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, sa_p, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
  return;
}

void listen_sock(int* sockfd_p){
  if (listen(*sockfd_p, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  return;
}


void close_sock(int* sockfd_p){
  close(*sockfd_p);
  return;
}
