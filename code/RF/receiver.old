#include <stdio.h>
#include <iostream>
#include <istream>
#include <fstream>
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
#include "NCpacket.h"
#include "utils_wsn.h" 
#include <cmath>
#include <ctgmath>
#include <vector>
#include <iterator>


#define RECEIVER_PORT "30000"
#define BACKLOG 10

/**
 * Send an ACK to the address in sockaddr_storage
 */
int sendack(unsigned int packets_needed, struct sockaddr_storage sender_addr) {
	// get info on sender
	int status;
	struct addrinfo ack_hints, *ack_res, *p_iter;
    memset(&ack_hints, 0, sizeof ack_hints);
    ack_hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    ack_hints.ai_socktype = SOCK_DGRAM;
    char s[INET6_ADDRSTRLEN];
    char p[2];
    inet_ntop(sender_addr.ss_family, get_in_addr((struct sockaddr *)&sender_addr), s, sizeof s);
    // std::cout << "host: " << s;
    std::string ack_port = std::to_string(ntohs(get_in_port((struct sockaddr *)&sender_addr)));
	// std::cout << "\nport: " << ack_port << "\n";
    if ((status = getaddrinfo(
    					s, 
    					ack_port.c_str(),
    					&ack_hints, &ack_res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
	}

	// at this point res is a linked structure filled with useful data
	// navigate it!
	int sockfd_ack; // socket file descriptor
	// let's get the socket and bind!
	for(p_iter = ack_res;p_iter != NULL; p_iter = p_iter->ai_next) {
		// make a socket for the first valid entry in the linked list
		// also check errors
		if((sockfd_ack = socket(p_iter->ai_family, p_iter->ai_socktype, 
			p_iter->ai_protocol)) == -1) {
			// there was an error, try with the next one
			perror("receiver ack: socket");
			continue;
		}
		break;
	}

	if (p_iter == NULL) { //no valid address was found
		std::cout << "receiver: unable to create ACK socket\n";
		return 2;
	}

	// send ack
	unsigned char *ack_buffer = (unsigned char *)calloc(sizeof(int), sizeof(char));
	packu32(ack_buffer, packets_needed);
	int byte_sent;
	int byte_to_send = sizeof(int);
	if((byte_sent = sendall(sockfd_ack, (char*)ack_buffer, &byte_to_send, p_iter)) == -1) {
		perror("receiver: ack socket");
		std::cout << "Sent only " << byte_sent << " byte because of an error\n";
	}
	return byte_sent;
}

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res, *p_iter;
	struct sockaddr_storage sender_addr;
    int status;

    if (argc != 2) {
    	std::cout << "usage: receiver filename\n";
    	return 2;
    }

    // set the whole hints to 0, then change what should be changed
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_DGRAM;
    // TODO localhost or this computer!!
    if ((status = getaddrinfo("localhost", RECEIVER_PORT, &hints, &res)) != 0) {
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

	if (p_iter == NULL) { //no valid address was found
		std::cout << "receiver: unable to create ACK socket\n";
		return 2;
	}

	printf("Ready to communicate\n");

	std::vector<NCpacket> nc_vector;
	int rec_bytes;
	socklen_t sizeof_sender_addr = sizeof sender_addr;
	// create receive buffer
	void* receive_buffer = malloc(PAYLOAD_SIZE*K_TB_SIZE*sizeof(char));
	// open file for the first time
	std::ofstream output_file (argv[1], std::ios::out | std::ios::binary);
	if (output_file.is_open()) {
		output_file.close();
	}
	int received_packets = 0;
	// find a conditions so that rx knows it has received the whole txed file
	while(true) {
		if((rec_bytes = recvfrom(sockfd, receive_buffer, PAYLOAD_SIZE+sizeof(int), 0, 
			(struct sockaddr*)&sender_addr, &sizeof_sender_addr))==-1) {
			// there was an error
			perror("receiver: recvfrom");
		}
		else
		{
			received_packets++;
			char s[INET6_ADDRSTRLEN];
			// std::cout << "Receiver: got packet from " << 
			// 	inet_ntop(sender_addr.ss_family, get_in_addr((struct sockaddr *)&sender_addr), s, sizeof s) <<"\n";
			// deserialize and save NCpacket into a vector
			char *data = (char *)receive_buffer;
			NCpacket packet = deserialize(data);
			nc_vector.push_back(packet);
			
			//check if K_TB_SIZE packets were received
			// TODO when using the encoding/decoding it is not guaranteed that K_TB_SIZE will be reached
			if(nc_vector.size() == K_TB_SIZE) {
				// decode
				// if decoding was successful, store packets' payload in file
				std::ofstream output_file (argv[1], std::ios::out | std::ios::app | std::ios::binary);
				if (output_file.is_open()) {
					for(std::vector<NCpacket>::iterator v_iter = nc_vector.begin(); v_iter != nc_vector.end(); ++v_iter) {
						output_file.write((char *)v_iter->getPayload(), PAYLOAD_SIZE);
					}
					output_file.close();
					nc_vector.clear();
				}
				else {
					std::cout << "Error in opening output_file";
					return 2;
				}
				// send ACK with the number of packets needed
				// TODO this ACK will specify how many encoded packets are still needed, check when it is optimal to send it
				unsigned int packets_needed = 0;
				sendack(packets_needed, sender_addr);
			}
		}
		std::cout << received_packets << "\n";
	}

	close(sockfd);

	return 0;

}