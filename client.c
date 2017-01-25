#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "udp.c"
#include "tcp.c"

#define MAX 80
#define PORT 43451

int main()
{

  bool tcp = true;

  pthread_t th, th2;

  int sockfd;
  size_t len;
  struct sockaddr_in servaddr;

  if(tcp)
    sockfd = socket(AF_INET,SOCK_STREAM,0);
  else
    sockfd = socket(AF_INET,SOCK_DGRAM,0);

  if(sockfd==-1) {
    printf("socket creation failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully created..\n");
  bzero(&servaddr,sizeof(len));
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
  servaddr.sin_port=htons(PORT);
  len=sizeof(servaddr);




  if(tcp){
    if(connect(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr)) !=0) {
      printf("connection with the server failed...\n");
      exit(0);
    }
    else
      printf("connected to the server..\n");

    if(pthread_create(&th, NULL, tcp_client, &sockfd) != 0){
      printf("asdf");
    }
    if(pthread_create(&th2, NULL, tcp_server, &sockfd) != 0){
      printf("bbb");
    }
    //func(sockfd);

    if(pthread_join(th, NULL)) {
      printf("%d\n", 2);
    }
    if(pthread_join(th2, NULL)) {
      printf("%d\n", 2);
    }

    close(sockfd);


  } else {

    arguments *args = (arguments *) malloc(sizeof *args);
    args->sockfd = sockfd;
    args->len = len;
    args->servaddr = servaddr;
    args->server = false;

    if(pthread_create(&th, NULL, client, args)){
      printf("%d\n", 12);
    }

    if(pthread_create(&th2, NULL, server, args)){
      printf("13");
    }

    if(pthread_join(th, NULL)) {
      printf("%d\n", 2);
    }
    if(pthread_join(th2, NULL)) {
      printf("%d\n", 2);
    }
  }
}
