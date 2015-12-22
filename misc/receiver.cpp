/*
** receiver.cpp -- receives a simple datagram
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>  
#include <iostream>
#include "unistd.h"




#define RECEIVER_PORT "30000"
#define BACKLOG 10
#define MAX_SIZE 10000

// get sockaddr, IPv4 or IPv6: (from beej's guide)
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res, *p_iter;
	struct sockaddr_storage sender_addr;
    int status;

    if (argc != 2) {
    	std::cout << "usage: receiver hostname";
    	std::cout << argv[1];
    }

    // set the whole ints to 0, then change what should be changed
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_DGRAM;
    if ((status = getaddrinfo(argv[1], RECEIVER_PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
	}
	// at this point res is a linked structure filled with useful data
	// navigate it!
	int sockfd; // socket file descriptor
	// let's get the socket and bind!
	for(p_iter = res;p_iter != NULL; p_iter = p_iter->ai_next) {
		// make a socket for the first valid entry in the linked list
		// also check errors
		if((sockfd = socket(p_iter->ai_family, p_iter->ai_socktype, 
			p_iter->ai_protocol)) == -1) {
			// there was an error, try with the next one
			perror("receiver: socket");
			continue;
		}

		// bind
		int bind_result;
		if ((bind_result = bind(sockfd, p_iter->ai_addr, 
			p_iter->ai_addrlen)) == -1) {
			// there was an error, try with the next one
			perror("receiver: bind");
			continue;
		}
		break;
	}

	printf("Ready to communicate\n");

	int rec_bytes;
	socklen_t sizeof_sender_addr = sizeof sender_addr;
	void* buffer = malloc(MAX_SIZE*sizeof(char));
	if((rec_bytes = recvfrom(sockfd, buffer, MAX_SIZE-1, 0, 
		(struct sockaddr*)&sender_addr, &sizeof_sender_addr))==-1) {
		// there was an error
		perror("receiver: recvfrom");
	}
	else
	{
		char s[INET6_ADDRSTRLEN];
		std::cout << "Receiver: got packet from " << 
			inet_ntop(sender_addr.ss_family, get_in_addr((struct sockaddr *)&sender_addr), s, sizeof s) <<"\n";
		char *data = (char *)buffer;
		data[MAX_SIZE] = '\0';
		std::string str(data);
		std::cout << str << "\n";
	}

	close(sockfd);

	return 0;

}