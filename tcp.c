#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#define MAX 80

void * tcp_client(void * args){
  char buff[MAX];
  int * sockfd = (int *) args;

  while(true){
    bzero(buff,sizeof(buff));
    for(int i = 0; i < MAX; i++){
      buff[i] = getchar();
      if(buff[i] == '\n')
        break;
    }
    write( *sockfd, buff, sizeof(buff));
  }
  printf("Here :(");
  return NULL;
}

void * tcp_server(void * args){
  char buff[MAX];
  int *sockfd = (int *) args;

  while(true){
    read(*sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
    if((strncmp(buff,"exit",4))==0){
      printf("Client Exit...\n");
      break;
    }
  }
  return NULL;
}
