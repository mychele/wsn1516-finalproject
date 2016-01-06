#include <stdio.h>
#include <limits.h>
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

#define RECEIVER_PORT "30000"

typedef std::pair<int, unsigned char> ackPayload;

void timeConversion(std::chrono::nanoseconds &d, timeval &tv )
{
  std::chrono::microseconds usec = std::chrono::duration_cast<std::chrono::microseconds>(d);
  if( usec <= std::chrono::microseconds(0) )
    tv.tv_sec = tv.tv_usec = 0;
  else
  {
    tv.tv_sec = usec.count()/1000000;
    tv.tv_usec = usec.count()%1000000;
  }
}

int sendPackets(std::vector<char*> input_vector, int packetNumber, int sockfd_send, 
	struct addrinfo *p_iter, unsigned char block_ID) {
	int sentPackets = 0;
	// encoding of packetNumber packets
	std::vector<NCpacket> packetVector;
	for (int enc_pck = 0; enc_pck < packetNumber; enc_pck++) {
        NCpacket nc = NCpacket(input_vector, block_ID);
        packetVector.push_back(nc);
    }

	if(packetVector.size() > 0) {
		// send these packets
		for(std::vector<NCpacket>::iterator pckIt = packetVector.begin(); pckIt != packetVector.end(); ++pckIt) {
			char *serializedPacket = pckIt->serialize();
			int byte_to_send = pckIt->getInfoSizeNCpacket();
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
	return sentPackets;
}

ackPayload receiveACK(int sockfd_send, std::chrono::nanoseconds timeout) {
	// things needed to receive ACKs
	int ack_rec_bytes;
	int packets_needed;
	unsigned char ack_block_ID;
	// create receive buffer
	void* ack_buffer = calloc(2*sizeof(int), sizeof(char));
	// select things
	struct timeval tv;
    timeConversion(timeout, tv);
    fd_set readfds; 
    FD_ZERO(&readfds);
    FD_SET(sockfd_send, &readfds);
	int select_ret = select(32, &readfds, NULL, NULL, &tv);
	if (select_ret > 0) {
        // socket has pending data to read
        if((ack_rec_bytes = recvfrom(sockfd_send, ack_buffer, 2*sizeof(int), 0, 
			NULL, 0))==-1) {
			// there was an error
			perror("sender ACK: recvfrom");
			packets_needed = -1;
		}
		else {
			unsigned char *receive_buffer = (unsigned char*)ack_buffer;
			packets_needed = unpacku32(receive_buffer); 
			ack_block_ID = *(receive_buffer + sizeof(int));
		}
    }
    else if (select_ret == 0) { // timeout!
        packets_needed = -1;
        ack_block_ID = 0;
    } else {
		packets_needed = -2;
        ack_block_ID = 0;
    }
    free(ack_buffer);
	ackPayload toBeReturned(packets_needed, ack_block_ID);
	return toBeReturned;
}



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
	// timeout value
	// TODO adapt this value to the time needed to receive an ACK in case of success
	auto timeout_span = std::chrono::nanoseconds(200);
	if(input_file) {
		// read file size
		// get length of file:
	    input_file.seekg (0, input_file.end);
	    unsigned int file_length = input_file.tellg(); // 4 byte
	    // set cursor at the beginning
	    input_file.seekg (0, input_file.beg);
	    												//include file size
	    int num_encoding_op = ceil((float)(file_length+sizeof(file_length))/(K_TB_SIZE*PAYLOAD_SIZE)); 
	    								// in TB of size K_TB_SIZE*PAYLOAD_SIZE byte
	    								// the last one may need padding, provided by calling calloc
	    int packet_needed_per_block_ID[UCHAR_MAX];

	    for(int encoding_op_index = 0; encoding_op_index < num_encoding_op; encoding_op_index++) { 
	    	unsigned char block_ID = (char) (encoding_op_index%UCHAR_MAX);
	    	unsigned char ack_block_ID;
	    	// create input buffer for K packets
			char *input_buffer; //PAYLOAD_SIZE and K_TB_SIZE are defined in NCpacket.h
			input_buffer = (char *)calloc(PAYLOAD_SIZE*K_TB_SIZE, sizeof(char));
			if(encoding_op_index == 0) { // first transmission
				// store file size in the first 4 byte
				packu32((unsigned char*) input_buffer, file_length);
				// read K_TB_SIZE*PAYLOAD_SIZE - sizeof(unsigned int) byte
				input_file.read((char *)input_buffer + sizeof(file_length), PAYLOAD_SIZE*K_TB_SIZE - sizeof(file_length));
			} else {
		    	// read K_TB_SIZE packets of PAYLOAD_SIZE byte
		    	input_file.read((char *)input_buffer, PAYLOAD_SIZE*K_TB_SIZE);
		    }
	    	std::vector<char *> input_vector = memoryToCharVector(input_buffer, K_TB_SIZE*PAYLOAD_SIZE);
	    	// TODO change this to N, and decide N
	    	unsigned int packets_needed = K_TB_SIZE+10; 
	    	packet_needed_per_block_ID[(int)block_ID] = packets_needed;
	    	do {
		    	// encode and send them
		    	sentPackets += sendPackets(input_vector, packets_needed, sockfd_send, p_iter, block_ID);
		    	// start measuring time to correctly receive an ACK
		    	auto tx_begin = std::chrono::high_resolution_clock::now();
		    	// wait for ACK, it will specify how many packets are needed
		    	// create promise
		    	std::packaged_task<ackPayload(int, std::chrono::nanoseconds)> waitForACK(&receiveACK);
		    	// get future
		    	std::future<ackPayload> packets_needed_future = waitForACK.get_future();
		    	// schedule on another thread
		    	std::thread th_ACK(std::move(waitForACK), sockfd_send, timeout_span);
		    	// check for timeout
		    	if(packets_needed_future.wait_for(timeout_span) == std::future_status::timeout) {
		    		// a timeout has occurred, no ACK was received
		    		// retransmit!
		    		// TODO consider if it is better to send K_TB_SIZE or less packets
		    		std::cout << "No ACK, retx\n";
		    		packets_needed = K_TB_SIZE;
		    		// increase timeout
		    		timeout_span += std::chrono::nanoseconds(100000);
		    	}
		    	else {
		    		// retransmit the number of packets specified
		    		ackPayload ack = packets_needed_future.get();
		    		packets_needed = ack.first;
		    		ack_block_ID = ack.second;
		    		packet_needed_per_block_ID[(int)ack_block_ID] = packets_needed;
		    		// change timeout_span dynamically only when an ACK is received after a tx
		    		auto tx_end = std::chrono::high_resolution_clock::now();
		    		timeout_span = std::chrono::duration_cast<std::chrono::nanoseconds>(tx_end-tx_begin);
		    	}
		    	th_ACK.join(); // clean up
	    	} while (packet_needed_per_block_ID[(int)block_ID] != 0);
	    	// free buffer
	    	free(input_buffer);
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