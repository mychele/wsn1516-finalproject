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
#include <chrono>
#include <future>
#include <thread>
#include <random>
#include "decoding_function.h"

#define RECEIVER_PORT "30000"
#define BACKLOG 10

/**
 * Struct used as return type of receivePacket
 */
struct ReceiveReturn
{
	NCpacket packet;
	struct sockaddr_storage sender_addr;
	int rec_bytes;
};

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
	close(sockfd_ack);
	return byte_sent;
}

/** 
 * Receive a packet from sockfd and return the NCpacket, the sender address
 * and the number of bytes received
 */
struct ReceiveReturn receivePacket (int sockfd, void* receive_buffer) {
	int rec_bytes = 0;
	NCpacket packet;
	// create the structure that will be filled with the sender address
	struct sockaddr_storage sender_addr;
	socklen_t sizeof_sender_addr = sizeof sender_addr;
	if((rec_bytes = recvfrom(sockfd, receive_buffer, packet.getInfoSizeNCpacket(), 0, 
		(struct sockaddr*)&sender_addr, &sizeof_sender_addr))==-1) {
		// there was an error
		perror("receiver: recvfrom");
	} else {
		packet = deserialize((char *)receive_buffer);
	}
	ReceiveReturn toBeReturned = ReceiveReturn();
	toBeReturned.sender_addr = sender_addr;
	toBeReturned.packet = packet;
	toBeReturned.rec_bytes = rec_bytes;
	return toBeReturned;
}

int main(int argc, char *argv[])
{
	// for testing and simulation
	std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 1); // define the range
    double PER = 0.1; // packet error rate

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
	
	// open file for the first time
	std::ofstream output_file (argv[1], std::ios::out | std::ios::binary);
	if (output_file.is_open()) {
		output_file.close();
	}
	int total_received_packets = 0;
	// timeout value
	std::chrono::seconds timeout_span(100); // to ultimately avoid deadlock
	// find a conditions so that rx knows it has received the whole txed file
	unsigned char rx_block_ID = 0;
	// create receive buffer
	void* receive_buffer = malloc(PAYLOAD_SIZE*K_TB_SIZE*sizeof(char));
	while(true) {
		int received_packets = 0;
		int packets_needed = K_TB_SIZE;
		while (packets_needed > 0) {
			// ------------------receive or timeout-------------------
			// create promise
	    	std::packaged_task<ReceiveReturn(int, void*)> waitForPacket(&receivePacket);
	    	// get future
	    	std::future<ReceiveReturn> received_future = waitForPacket.get_future();
	    	// schedule on another thread
	    	std::thread th_packet(std::move(waitForPacket), sockfd, receive_buffer);
	    	// check for timeout
	    	if(received_future.wait_for(timeout_span) == std::future_status::timeout) {
	    		// a timeout has occurred, no packet was received for too long
	    		// and this group of packets was not decoded yet
	    		// the sender will timeout and retransmit automatically before this timer expires
	    		std::cout << "No packets for too long, stop execution\n";
	    		th_packet.detach();
	    		close(sockfd);
	    		return 2;
	    	}
	    	else {
	    		int received_byte;
	    		ReceiveReturn received = received_future.get();
	    		if (received.rec_bytes == -1) {
	    			// do not store the packet in vector, since there was an error
	    		} else {
	    			if(received.packet.getBlockID() == rx_block_ID) {
		    			nc_vector.push_back(received.packet);
		    			sender_addr = received.sender_addr;
		    			received_packets++;
	    			}
	    		}
	    	}
	    	th_packet.detach(); // clean up

	    	//------------------------------
			if(received_packets >= K_TB_SIZE) {
				// try to decode and update packets needed
				packets_needed = packet_decoder(nc_vector, argv[1]);
				std::cout << "packets_needed " << packets_needed << "\n";
				// TODO remove this and move it to a packet level when decoding is in place
				// this simulates an error and drops K_TB_SIZE packets, when decoding is used
				// it will be possible to drop a single packet
				// double prob = distr(eng);
				// if (prob < PER) {
				// 	nc_vector.clear();
				// 	packets_needed = K_TB_SIZE;
				// 	received_packets = 0;
				// } 
				rx_block_ID = (packets_needed == 0) ? (rx_block_ID = (rx_block_ID+1)%UCHAR_MAX) : rx_block_ID;
				sendack(packets_needed, sender_addr); 
			}
		}
		nc_vector.clear();
		total_received_packets += received_packets;
		std::cout << total_received_packets << "\n";
	}

	free(receive_buffer);
	close(sockfd);

	return 0;

}