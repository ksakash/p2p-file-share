/*
** tcp_client.c -- a simple tcp client
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 3490 // the port client will be connecting to 

#define MAXBUFLEN 256 // max number of bytes we can get at once 

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXBUFLEN];
	struct hostent *he;
	struct sockaddr_in their_addr; // connector's address information 

	if (argc != 2) {
	    fprintf(stderr,"usage: tcp_client hostname\n");
	    exit(1);
	}


	if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info 
	    perror("gethostbyname");
	    exit(1);
	}
	
	// create a tcp socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    perror("socket");
	    exit(1);
	}
	
	// fill in server info
	their_addr.sin_family = AF_INET;    // host byte order 
	their_addr.sin_port = htons(PORT);  // short, network byte order 
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

	// make a tcp connection to the server
	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof their_addr) == -1) {
	    perror("connect");
	    exit(1);
	}
	
	// receive data from the server
	if ((numbytes=recv(sockfd, buf, MAXBUFLEN-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("Received: %s",buf);

	close(sockfd);

	return 0;
}
