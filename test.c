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

#define OUTPUT stdout
#define REQUESTS 10
#define KEYWORD "PING"
#define HOST argv[1]
#define PORT argv[2]
#define SERVER res_ref->ai_addr
#define MAXSIZE sizeof(KEYWORD)+2+(int)(floor(log10(LONG_MAX))+1)
#define TIME(start, stop) 1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000

#define INVALID "invalid or missing options\nusage: snc [-l] [-u] [-s source_ip_address] [hostname] port\n"

void errors(int i){
	switch(i){
		case 1: fprintf(stderr, "Usage: ./PingClient host port\n"); 							exit(1);
		case 2: fprintf(stderr, "Can't create a socket\n"); 											exit(2);
		case 3: fprintf(stderr, "Can't get server's address info\n"); 						exit(3);
		case 4: fprintf(stderr, "Could not modify datagram to allow timeout\n");	exit(4);
		default: exit(5);
	}
}

void invalid_message(){
  fprintf(stderr, INVALID);
  exit(1);
}

void *udplistener(int port) {
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1) errors(1)

	struct sockaddr_in serveraddr;
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short) atoi(port));

	if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		errors(2)

	struct sockaddr_in clientaddr;
	socklen_t clientlen;
	int n;
	char buf[100];
	clientlen = sizeof(clientaddr);
	bzero(buf, 100);
	n = recvfrom(sockfd, buf, 100, 0, (struct sockaddr *) &clientaddr, &clientlen);
	if (n < 0) errors(3)

	struct hostent *hostp;
	hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	if (hostp == NULL) errors(4)

	char * hostaddrp;
	hostaddrp = inet_ntoa(clientaddr.sin_addr);
	if (hostaddrp == NULL) errors(5)

	printf("server received datagram from %s (%s)\n", hostp->h_name, hostaddrp);
	printf("server received %lu/%d bytes: %s\n", strlen(buf), n, buf);

}

void *udpsender(int port, char * hostname){
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1) errors(1)

	// Get addr info, used to bind() and connect()
	struct addrinfo *res_ref, hints = {0, AF_INET, SOCK_DGRAM};
	if (getaddrinfo(hostname, port, &hints, &res_ref)) errors(2)

	if (sendto(sockfd, "init", sizeof("init"), 0, SERVER, sizeof(struct sockaddr)) == -1)
		errors(3)
}

typedef struct {
    int * sockfd;
    int * sock_addr_size;
		struct addrinfo **res_ref;
} updsenderinfo;

void *udpsender(void * args){
	
	uppsenderinfo * data = args;
	int sockfd = *data->sockfd;
	int sock_addr_size = *data->sock_addr_size;
	struct addrinfo *res_ref = *data->res_ref;

	char buffer[100];
	while(true){
		scanf("%s", buffer);
		if(sendto(sockfd, buffer, strlen(buffer), 0, SERVER, sock_addr_size) == -1)
			errors(1)
	}
	free(data)
}



bool listen_server = false, udp = false, source = false;
char * source_ip_address, * port, * hostname = NULL, * flag_position[3];

void serve();
void client();
pthread_t tid[2];
// 0: name, 1: host, 2: port
// snc [-l] [-u] [-s source_ip_address] [hostname] port
int main(int argc, char **argv) {

  // PARSE INPUT
	printf("%d\n", argc);

  // Handle all options
  for(int i = 1; i < argc; i++){ // First argv will be "snc"
    switch(argv[i][1]){ // Get the second character (ignore the dash)
      case 'l':
        listen_server = true;
        flag_position[0] = argv[i];
        break;
      case 'u':
        udp = true;
        flag_position[1] = argv[i];
        break;
      case 's':
        source = true;
        flag_position[2] = argv[i];
        source_ip_address = argv[i+1];
    }
  }
  port = argv[argc-1];
  if (source_ip_address != argv[argc-2]) {
    bool no_host = false;
    for(int i = 0; i < 3; i++){
      if (argv[argc-2] == flag_position[i])
        no_host = true;
    }
    if (! no_host)
    hostname = argv[argc-2];
  }

  // Check to make sure everything is gucci:
  for(int i = 0; i < 3; i++){
    if(argv[argc-1] == flag_position[i])
      invalid_message();
  }
  if(source_ip_address == port)
    invalid_message();

  if(listen_server && source)
    invalid_message();

  if(!listen_server && !hostname)
    invalid_message();


  // Connect to the other machine
  if(listen_server){
  }
  else {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1){
      // Handle error
			error(1)
    }

    // Get addr info, used to bind() and connect()
    struct addrinfo *res_ref, hints = {0, AF_INET, SOCK_DGRAM};
    if (getaddrinfo(hostname, port, &hints, &res_ref)) {
      // Handle error
			error(2)
    }


    if (sendto(sockfd, "init", sizeof("init"), 0, SERVER, sizeof(struct sockaddr)) == -1){
      // Handle error
    }

    pthread_t id = pthread_self();
    if(pthread_equal(id,tid[0]))
      serve();
    else
      client();

  }

  return 0;
/*
	struct addrinfo *res_ref, hints = {0, AF_INET, SOCK_DGRAM};
	struct timeval start = {1}, end;
	char message[MAXSIZE], receive[MAXSIZE];
	uint addrlen = sizeof(struct sockaddr);
	int i, err=0, min=1000, average, max=0, times[REQUESTS], sockfd=socket(AF_INET, SOCK_DGRAM, 0);

	// Check arguments
	if (argc != 3) errors(1);
	// Create UDP datagram
	if (sockfd == -1) errors(2);
	// Get a pointer to *res
	if (getaddrinfo(HOST, PORT, &hints, &res_ref)) errors(3);
	// Allow timeout
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &start, sizeof(start)) == -1) errors(4);

	for(i = 0; i < REQUESTS; i++){
		// Construct Message
		snprintf(message, MAXSIZE, "%s %i %li", KEYWORD, i, start.tv_sec);
		// Start timer
		gettimeofday(&start, NULL);
		// Send & Receive
		if (sendto(sockfd, message, sizeof(message), 0, SERVER, sizeof(struct sockaddr)) == -1
				|| recvfrom(sockfd, receive, sizeof(receive), 0, SERVER, &addrlen) == -1
				|| receive[5]-48 != i){
			times[i] = -1;
			err++;
			continue;
		}
		// Stop Timer
		gettimeofday(&end, NULL);
		times[i] = TIME(start, stop);
		if(times[i] < min) min = times[i];
		if(times[i] > max) max = times[i];
		// Print
		fprintf(OUTPUT, "PING received from %s: seq#=%c time=%i ms\n", HOST, receive[5], times[i]);
	}

	// Calculate average
	for(i = 0; i < REQUESTS; i++) if(times[i] >= 0) average += times[i]; average /= (REQUESTS-err);

	fprintf(OUTPUT, "--- %s ping statistics ---\n", HOST);
	char s[] = "%i packets transmitted, %i received, %i%% packet loss rtt min/avg/max = %i %i %i ms\n";
	fprintf(OUTPUT, s, REQUESTS, REQUESTS-err, (int)((err/(float)REQUESTS)*100), min, average, max);
*/
}

void serve(){

}

void client(){
}
