#include<stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX 1024

void * tcp_client(void * args){

  char buff[MAX] = "a";
  int * sockfd = (int *) args;
  int i = 0;

  while(true){
    bzero(buff,sizeof(buff));
    for(i = 0; i < MAX; i++){
      buff[i] = getchar();
      if(buff[i] == '\n')
        break;
      if(buff[i] == EOF){
        buff[0] = EOF;
        write(*sockfd, buff, sizeof(buff));
        exit(0);
      }
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
    if(buff[0] == EOF)
      exit(0);
    printf("%s", buff);
  }
  return NULL;
}
