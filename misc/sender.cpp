#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>  
#include <string>
#include <cstring>
#include "unistd.h"


#define RECEIVER_PORT "30000"
#define SENDER_PORT "30001"
#define MAX_SIZE 10000

int main(int argc, char const *argv[])
{
	// read input
	if (argc != 3) {
		std::cout << "usage: sender hostname dstname";
	}

	// useful structs and variables
	struct addrinfo hints, *res, *p_iter, dst, *res_dst;
	int status;

	// create socket and bind in order to receive ACK
	// set the whole hints to 0, then change what should be changed
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_DGRAM;
    // hints.ai_flags = AI_PASSIVE; // this should set the address as the one of this computer
    if ((status = getaddrinfo(argv[1], SENDER_PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
	}
	// at this point res is a linked structure filled with useful data
	// navigate it!
	int sockfd_listen; // socket file descriptor
	// let's get the socket and bind!
	for(p_iter = res;p_iter != NULL; p_iter = p_iter->ai_next) {
		// make a socket for the first valid entry in the linked list
		// also check errors
		if((sockfd_listen = socket(p_iter->ai_family, p_iter->ai_socktype, 
			p_iter->ai_protocol)) == -1) {
			// there was an error, try with the next one
			perror("sender: socket");
			continue;
		}

		// bind
		int bind_result;
		if ((bind_result = bind(sockfd_listen, p_iter->ai_addr, 
			p_iter->ai_addrlen)) == -1) {
			// there was an error, try with the next one
			perror("sender: bind");
			continue;
		}
		break;
	}

	if (p_iter == NULL) { //no valid address was found
		std::cout << "sender: unable to create ACK socket\n";
		return 2;
	}

	freeaddrinfo(res);

	// create socket in order to send data to receiver
	memset(&dst, 0, sizeof dst);
	dst.ai_family = AF_UNSPEC;
	dst.ai_socktype = SOCK_DGRAM;
	if ((status = getaddrinfo(argv[2], RECEIVER_PORT, &dst, &res_dst)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
	}
	std::cout << "Ready to communicate, insert your message\n";

	std::string message;
	// read input
	getline(std::cin, message);

	std::cout << "The message you want to send is " << message << "\n";
	int byte_to_send = message.length();
	int byte_sent;

	const char* str_pointer = message.c_str();
	int sockfd_send;
	// send input
	for (p_iter = res_dst; p_iter != NULL; p_iter = p_iter->ai_next) {
		// create a socket
		if((sockfd_send = socket(p_iter->ai_family, p_iter->ai_socktype, 
			p_iter->ai_protocol)) == -1) {
			// there was an error, try with the next one
			perror("sender: tx socket");
			continue;
		}
		break;
	}

	if (p_iter == NULL) {
		std::cout << "sender: unable to create send socket\n";
		return 2;
	}

	if((byte_sent = sendto(sockfd_send, str_pointer, byte_to_send, 0, 
		p_iter->ai_addr, p_iter->ai_addrlen)) == -1) {
		perror("sender: sendto");
		exit(1); 
	}
	
	if(byte_sent >= 0) { // this means that at least a tx was done

		while(byte_sent < byte_to_send)
		{
			str_pointer += byte_sent;
			byte_sent = sendto(sockfd_send, str_pointer, byte_to_send-byte_sent, 0, 
			p_iter->ai_addr, p_iter->ai_addrlen);
			if(byte_sent == -1) {
				perror("sender: completing sendto");
				return 2;
			}
			std::cout << "Sent " << byte_sent << " bytes\n"; 
		}
	}

	freeaddrinfo(res_dst);

	// close sockets
	close(sockfd_listen);
	close(sockfd_send);
	return 0;
}