#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX 80

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
    }
    sendto(data->sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&data->servaddr, data->len);
  }
  return NULL;
}

void * server(void * args){
  arguments * data = (arguments *) args;
  char buff[MAX];
  //bool init = true;

  while(true){
    recvfrom(data->sockfd,buff,sizeof(buff),0,(struct sockaddr *)&data->servaddr,&data->len);
    // if(init && data->server){
    //   connect(data->sockfd, (struct sockaddr *)&data->servaddr, data->len);
    //   init = false;
    // }

    printf("%s",buff);
  }
  return NULL;
}
