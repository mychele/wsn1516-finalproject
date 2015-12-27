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
#define SENDER_PORT "30001"
#define MAX_SIZE 10000


int sendall(int sockfd_send, char *send_buffer, int *byte_to_send, addrinfo *p_iter) {

	int total_byte_sent = 0;
	int byte_sent = 0;
	int byte_left = *byte_to_send;

	while (total_byte_sent < *byte_to_send) {
		byte_sent = sendto(sockfd_send, send_buffer+total_byte_sent, 
			byte_left, 0, p_iter->ai_addr, p_iter->ai_addrlen);
		if(byte_sent == -1) {
			break;
		}
		total_byte_sent += byte_sent;
		byte_left -= byte_sent;
	}
	
	*byte_to_send = total_byte_sent;
	return byte_sent==-1? -1:0; // return failure or success
}



int main(int argc, char const *argv[])
{
	// read input
	if (argc != 3) {
		std::cout << "usage: sender hostname dstname";
		return 2;
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

	// read the file, segment it into packets of 1024 bytes and pass them to the encoder in groups of K=32
	// the best approach is to load the file in memory and then pass references to these memory areas, 
	// get back pointers to NCpacket objects and send them

	// create send socket
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

	std::ifstream input_file ("../testfiles/divina_commedia.txt", std::ifstream::binary);
	std::cout << "K " << K_TB_SIZE << "\n";
	int sentPackets = 0;
	if(input_file) {
		// read file size
		// get length of file:
	    input_file.seekg (0, input_file.end);
	    int file_length = input_file.tellg();
	    // set cursor at the beginning
	    input_file.seekg (0, input_file.beg);

	    int num_encoding_op = ceil((float)file_length/(K_TB_SIZE*PAYLOAD_SIZE)); // in TB of size K_TB_SIZE*PAYLOAD_SIZE byte
	    								// the last one may need padding, provided by calling calloc
	    for(int encoding_op_index = 0; encoding_op_index < num_encoding_op; encoding_op_index++) { 
	    	// create input buffer for K packets
			char *input_buffer; //PAYLOAD_SIZE and K_TB_SIZE are defined in NCpacket.h
			input_buffer = (char *)calloc(PAYLOAD_SIZE*K_TB_SIZE, sizeof(char));
	    	// read K_TB_SIZE packets of PAYLOAD_SIZE byte
	    	input_file.read((char *)input_buffer, PAYLOAD_SIZE*K_TB_SIZE);
	    	// encode them
	    	// the encoding operation should return a std::vector of NCpackets
	    	// send them
	    	std::vector<NCpacket> packetVector = memoryToVector(input_buffer, K_TB_SIZE*PAYLOAD_SIZE);
	    	if(packetVector.size() > 0) {
	    		// send these packets
	    		for(std::vector<NCpacket>::iterator pckIt = packetVector.begin(); pckIt != packetVector.end(); ++pckIt) {
	    			char *serializedPacket = pckIt->serialize();
	    			int byte_to_send = PAYLOAD_SIZE + sizeof(int);
					int byte_sent;
					if((byte_sent = sendall(sockfd_send, serializedPacket, &byte_to_send, p_iter)) == -1) {
						perror("sender: tx socket");
						std::cout << "Sent only " << byte_sent << " byte because of an error\n";
					}
					free(serializedPacket);
					sentPackets++;
	    		}
	    		packetVector.clear();
	    	}
	    	// free buffer
	    	free(input_buffer);

	    	// TODO
	    	// wait for ACK, it will specify how many packets are needed

	    }
	}
	else {
		std::cout << "error in reading input file";
		return 2;
	}

	std::cout << sentPackets <<"\n";

	freeaddrinfo(res_dst);

	// close sockets
	close(sockfd_listen);
	close(sockfd_send);
	return 0;
}