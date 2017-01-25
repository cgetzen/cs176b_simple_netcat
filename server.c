#include<stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "udp.c"
#include "tcp.c"

void errors(int i){
  switch(i){
    case 1: printf("socket creation failed...\n"); break;
    case 2: printf("socket bind failed...\n"); break;
    case 3: printf("Listen failed...\n"); break;
    case 4: printf("server acccept failed...\n"); break;
    case 5: printf("Creating thread failed...\n"); break;
    case 6: printf("Joining thread failed...\n"); break;
  }
  exit(i);
}

// snc [-l] [-u] [-s source_ip_address] [hostname] port
int main(int argc, char * argv[]) {
  extern char *optarg;
  bool tcp = true, s_mode = false, source_ip = false, hostname = false;
  char *source_ip_address;
  char c;
  while ((c = getopt(argc, argv, "lus:")) != -1)
		switch (c) {
      case 'l': s_mode = true; break;
      case 'u': tcp = false; break;
      case 's': source_ip = true; source_ip_address = optarg; break;
    }
  if(argv[argc-2] != "-" && argv[argc-2] != source_ip_address && argc > 2)
    hostname = true;
  int port = atoi(argv[argc-1]);

  pthread_t thread_1, thread_2;

  int sockfd = tcp ? socket(AF_INET,SOCK_STREAM,0) : socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd==-1) errors(1);

  struct sockaddr_in servaddr;
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(port);

  if(s_mode){
    // SERVER CODE
    if((bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))!=0) errors(2);

    if(tcp) {
      // TCP SERVER CODE
      if((listen(sockfd, 5)) != 0) errors(3);

      struct sockaddr_in serveraddr;
      socklen_t len=sizeof(serveraddr);
      int tcpsockfd=accept(sockfd,(struct sockaddr *)&serveraddr, &len);
      if(tcpsockfd<0) errors(4);


      if(pthread_create(&thread_2, NULL, tcp_client, &tcpsockfd)) errors(5);
      if(pthread_create(&thread_1, NULL, tcp_server, &tcpsockfd)) errors(5);

      if(pthread_join(thread_1, NULL)) errors(6);
      if(pthread_join(thread_2, NULL)) errors(6);

      close(tcpsockfd);

    } else {
      // UDP SERVER CODE
      struct sockaddr_in serveraddr;

      arguments *args = (arguments *) malloc(sizeof *args);
      args->sockfd = sockfd;
      args->len = sizeof(serveraddr);
      args->servaddr = serveraddr;
      args->server = true;

      if(pthread_create(&thread_2, NULL, client, args)) errors(5);
      if(pthread_create(&thread_1, NULL, server, args)) errors(5);

      if(pthread_join(thread_1, NULL)) errors(6);
      if(pthread_join(thread_2, NULL)) errors(6);
      close(sockfd);
    }
  }
  else {
    // CLIENT CODE
    // Bind if given -s option
    if(source_ip){
      inet_aton(source_ip_address, (struct in_addr *)&servaddr);
      if((bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))!=0) errors(2);
    }

    if(tcp){
      // TCP CLIENT CODE
      if(connect(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr)) !=0) {
        printf("connection with the server failed...\n");
        exit(0);
      }
      else
        printf("connected to the server..\n");

      if(pthread_create(&thread_1, NULL, tcp_client, &sockfd) != 0) errors(5);
      if(pthread_create(&thread_2, NULL, tcp_server, &sockfd) != 0) errors(5);
      //func(sockfd);

      if(pthread_join(thread_1, NULL)) errors(6);
      if(pthread_join(thread_2, NULL)) errors(6);
      close(sockfd);
    }
    else {
      // UDP CLIENT CODE
      arguments *args = (arguments *) malloc(sizeof *args);
      args->sockfd = sockfd;
      args->len = sizeof(servaddr);
      args->servaddr = servaddr;
      args->server = false;

      if(pthread_create(&thread_1, NULL, client, args)) errors(5);
      if(pthread_create(&thread_2, NULL, server, args)) errors(5);

      if(pthread_join(thread_1, NULL)) errors(6);
      if(pthread_join(thread_2, NULL)) errors(6);
    }

  }
}
