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

int main(int argc, char const *argv[])
{
	// read input
	if (argc != 4) {
		std::cout << "usage: sender hostname dstname filename";
		return 2;
	}

	// useful structs and variables
	struct addrinfo *p_iter, dst, *res_dst;
	int status;

	// things needed to receive ACKs
	int ack_rec_bytes;
	struct sockaddr_storage ack_addr;
	socklen_t sizeof_ack_addr = sizeof ack_addr;
	// create receive buffer
	void* ack_buffer = calloc(2*sizeof(int), sizeof(char));

	// create socket in order to send data to receiver
	memset(&dst, 0, sizeof dst);
	dst.ai_family = AF_UNSPEC;
	dst.ai_socktype = SOCK_DGRAM;
	if ((status = getaddrinfo(argv[2], RECEIVER_PORT, &dst, &res_dst)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
	}

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

	// get the port from which packets will be sent


	std::ifstream input_file (argv[3], std::ifstream::binary);
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

	    	// wait for ACK, it will specify how many packets are needed
	    	// the socket is already bound
	    	unsigned int packets_needed;
	    	if((ack_rec_bytes = recvfrom(sockfd_send, ack_buffer, 2*sizeof(int), 0, 
				(struct sockaddr*)&ack_addr, &sizeof_ack_addr))==-1) {
				// there was an error
				perror("sender ACK: recvfrom");
			}
			else {
				packets_needed = unpacku32((unsigned char *)ack_buffer); // TODO encode and retx the 
																		 // number of packets specified
			}

	    }
	}
	else {
		std::cout << "error in reading input file";
		return 2;
	}

	std::cout << sentPackets <<"\n";

	freeaddrinfo(res_dst);
	// close socket
	close(sockfd_send);
	return 0;
}