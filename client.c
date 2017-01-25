#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "udp.c"

#define MAX 80
#define PORT 43454

int main()
{
  pthread_t th, th2;

  char buff[MAX];
  int sockfd,n;
  size_t len;
  struct sockaddr_in servaddr;
  sockfd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd==-1)
  {
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


  arguments *args = malloc(sizeof *args);
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
