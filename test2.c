#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>
#include <limits.h>

#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>

typedef struct {
    int * sockfd;
    int * sock_addr_size;
    struct addrinfo **res_ref;
} udpsenderinfo;

typedef struct{
  int * sockfd;
  socklen_t * clientlen;
  struct sockaddr_in *clientaddr;

} udpreceiverinfo;

void * udpsender(void * args){

  udpsenderinfo * data = args;
  int sockfd = *data->sockfd;
  int sock_addr_size = *data->sock_addr_size;
  struct addrinfo *res_ref = *data->res_ref;

  char buffer[100];
	while(true){
    fgets(buffer, sizeof buffer, stdin);
		if(sendto(sockfd, buffer, strlen(buffer), 0, res_ref->ai_addr, sock_addr_size) == -1)
			printf("16");
	}

	free(data);
  return NULL;
}

void * udpreceiver(void * args){
  udpreceiverinfo * data = args;
  int sockfd = *data->sockfd;
  socklen_t clientlen = *data->clientlen;
  struct sockaddr_in clientaddr = *data->clientaddr;

  char buf[100];
  int n;
  while(true){
    bzero(buf, 100);
    n = recvfrom(sockfd, buf, 100, 0, (struct sockaddr *) &clientaddr, &clientlen);
    printf("%s", buf);
    //printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
    //printf("server received %lu/%d bytes: %s\n", strlen(buf), n, buf);
  }
}

void startserver();
void startclient();

int main(int argc, char *argv[]){
  pthread_t th;
  if(argc > 1){
    startserver();
  } else {
    startclient();
  }
}

void startclient(){
  pthread_t th;
  struct addrinfo *res_ref, hints = {0, AF_INET, SOCK_DGRAM};
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int size  = sizeof(struct sockaddr);

  if(sockfd == -1){
    printf("10");
  }
  // Get addr info, used to bind() and connect()
  if (getaddrinfo("localhost", "5000", &hints, &res_ref)) {
    printf("11");
  }

  udpsenderinfo *args = malloc(sizeof *args);
  args->sockfd = &sockfd;
  args->sock_addr_size = &size;
  args->res_ref = &res_ref;
  if(pthread_create(&th, NULL, udpsender, args)){
    printf("%d\n", 12);
  }

  struct sockaddr_in clientaddr;

  // udpreceiverinfo *args2 = malloc(sizeof *args2);
  // args2->sockfd = &sockfd;
  // args2->clientlen = &size;
  // args2->clientaddr
  char receive[100];
  uint addrlen = sizeof(struct sockaddr);
  recvfrom(sockfd, receive, sizeof(receive), 0, res_ref->ai_addr, &addrlen);

  if(pthread_join(th, NULL)) {
    printf("%d\n", 22);
  }

}

void startserver(){
  pthread_t th[2];
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1) printf("1");

	struct sockaddr_in serveraddr;
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short) atoi("5000"));

	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		printf("14");

	struct sockaddr_in clientaddr;
	socklen_t clientlen = sizeof(clientaddr);
	int n;
	char buf[100];

	bzero(buf, 100);
	n = recvfrom(sockfd, buf, 100, 0, (struct sockaddr *) &clientaddr, &clientlen);
  connect(sockfd, (struct sockaddr *) &clientaddr, sizeof(clientaddr));
	if (n < 0) printf("-1");

	struct hostent *hostp;
	hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	if (hostp == NULL) printf("3");

	char * hostaddrp;
	hostaddrp = inet_ntoa(clientaddr.sin_addr);
	if (hostaddrp == NULL) printf("7");

  printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
  printf("server received %lu/%d bytes: %s\n", strlen(buf), n, buf);

  udpreceiverinfo *args = malloc(sizeof *args);
  args->sockfd = &sockfd;
  args->clientlen = &clientlen;
  args->clientaddr = &clientaddr;

  struct addrinfo *res_ref, hints = {0, AF_INET, SOCK_DGRAM};
  getaddrinfo(hostaddrp, "5000", &hints, &res_ref);
  udpsenderinfo *args2 = malloc(sizeof *args);
  args2->sockfd = &sockfd;
  args2->sock_addr_size = (int *) &clientlen;
  args2->res_ref = &res_ref;

  if(pthread_create(&th[0], NULL, udpreceiver, args)){
    printf("%d\n", 12);
  }

  if(pthread_create(&th[1], NULL, udpsender, args2)){
    printf("%d\n", 13);
  }

  if(pthread_join(th[0], NULL)) {
    printf("%d\n", 2);
  }
  if(pthread_join(th[1], NULL)) {
    printf("%d\n", 2);
  }

}
