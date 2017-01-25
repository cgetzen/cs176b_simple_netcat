#include<stdlib.h>
#include <pthread.h>
#include "udp.c"


#define PORT 43454


void func(int sockfd)
{
  pthread_t th, th2;

  int clen;
  struct sockaddr_in cli;
  clen=sizeof(cli);

  arguments *args = malloc(sizeof *args);
  args->sockfd = sockfd;
  args->len = clen;
  args->servaddr = cli;
  args->server = true;

  if(pthread_create(&th2, NULL, client, args)){
    printf("%d\n", 12);
  }

  if(pthread_create(&th, NULL, server, args)){
    printf("13");
  }


  if(pthread_join(th, NULL)) {
    printf("%d\n", 2);
  }
  if(pthread_join(th2, NULL)) {
    printf("%d\n", 2);
  }
}

int main()
{
  int sockfd;
  struct sockaddr_in servaddr;
  sockfd=socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd==-1)
  {
    printf("socket creation failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully created..\n");
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(PORT);
  if((bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)))!=0)
  {
    printf("socket bind failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully binded..\n");

  func(sockfd);
  //close(sockfd);
}
