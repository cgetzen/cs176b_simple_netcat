#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>
#include <limits.h>

#define OUTPUT stdout
#define REQUESTS 10
#define KEYWORD "PING"
#define HOST argv[1]
#define PORT argv[2]
#define SERVER res_ref->ai_addr
#define MAXSIZE sizeof(KEYWORD)+2+(int)(floor(log10(LONG_MAX))+1)
#define TIME(start, stop) 1000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/1000

void errors(int i){
	switch(i){
		case 1: fprintf(stderr, "Usage: ./PingClient host port\n"); 	exit(1);
		case 2: fprintf(stderr, "Can't create a socket\n"); 		exit(2);
		case 3: fprintf(stderr, "Can't get server's address info\n"); 	exit(3);
		case 4: fprintf(stderr, "Could not modify datagram to allow timeout\n");	exit(4);
		default: exit(5);
	}
}

// 0: name, 1: host, 2: port
int main(int argc, char **argv)
{
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

}
