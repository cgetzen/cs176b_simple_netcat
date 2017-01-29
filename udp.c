#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX 1024

typedef struct {
    int sockfd;
    socklen_t len;
    struct sockaddr_in servaddr;
    bool server;
} arguments;

void * client(void * args){
  arguments * data = (arguments *) args;
  char buff[MAX];

  while(true){
    bzero(buff,sizeof(buff));
    for(int i = 0; i < MAX; i++){
      buff[i] = getchar();
      if(buff[i] == '\n')
        break;
      if(buff[i] == EOF){
        goto END;
        buff[0] = EOF;
        sendto(data->sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&data->servaddr, data->len);
        exit(0);
      }
    }
    sendto(data->sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&data->servaddr, data->len);
  } END:
  return NULL;
}

void * server(void * args){
  arguments * data = (arguments *) args;
  char buff[MAX];

  while(true){
    recvfrom(data->sockfd,buff,sizeof(buff),0,(struct sockaddr *)&data->servaddr,&data->len);
    if(connect(data->sockfd, (struct sockaddr *)&data->servaddr,sizeof(data->servaddr))!=0)
      printf("oh no");

    printf("%s",buff);
  }
  return NULL;
}
