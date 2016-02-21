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
	struct timeval tv = timeConversion(std::chrono::duration_cast<std::chrono::microseconds>(timeout));
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
	bool verb = 0;

	srand(time(NULL));
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

	std::ifstream input_file (argv[3], std::ifstream::binary);
	std::cout << "K " << K_TB_SIZE << "\n";
	std::chrono::time_point<std::chrono::system_clock> start_file_tx, end_file_tx;
	int sentPackets = 0;
	double PPoverhead = 0;
	double PER_estimate = 0;
	bool no_history = 1;
	double alpha = 0.9;
	int PPO_history = 100;
	// timeout value
	// TODO adapt this value to let the sender respond to a deadlock (if no ACK received)
	auto timeout_span = std::chrono::seconds(10);
	bool PER_mode = 0;
	unsigned int file_length;
	int const N=K_TB_SIZE+5;

	start_file_tx = std::chrono::system_clock::now();
	if(input_file) {
		// read file size
		// get length of file:
	    input_file.seekg (0, input_file.end);
	    file_length = input_file.tellg(); // 4 byte
	    // set cursor at the beginning
	    input_file.seekg (0, input_file.beg);
	    												//include file size
	    int num_encoding_op = ceil((float)(file_length+sizeof(file_length))/(K_TB_SIZE*PAYLOAD_SIZE));
	    								// in TB of size K_TB_SIZE*PAYLOAD_SIZE byte
	    								// the last one may need padding, provided by calling calloc
	    int packet_needed_per_block_ID[UCHAR_MAX];
	    int packet_sent_per_block_ID[UCHAR_MAX];

	    for(int encoding_op_index = 0; encoding_op_index < num_encoding_op; encoding_op_index++) {
	    	unsigned char block_ID = (char) (encoding_op_index%UCHAR_MAX);
	    	unsigned char ack_block_ID = block_ID+1;
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
	    	unsigned int packets_needed = N;
	    	packet_needed_per_block_ID[(int)block_ID] = packets_needed;
	    	if(verb){std::cout << "send " << packet_needed_per_block_ID[(int)block_ID] <<"\n";}
	    	packet_sent_per_block_ID[block_ID] = sendPackets(input_vector, (PER_mode ? std::ceil((double)packets_needed/(1-PER_estimate)) : packets_needed), sockfd_send, p_iter, block_ID);
	    	do {
	    		if (ack_block_ID == block_ID) { // if the ACK just received is for this block
		    		// encode and send them
		    		if(verb){std::cout << "send " << packet_needed_per_block_ID[(int)block_ID] << "\n";}
		    		packet_sent_per_block_ID[block_ID] += sendPackets(input_vector, (PER_mode ? std::ceil((double)packets_needed/(1-PER_estimate)) : packets_needed), sockfd_send, p_iter, block_ID);
		    	}
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
		    		std::cout << "No ACK, retx\n";
		    		packets_needed = N;
		    	}
		    	else {
		    		// retransmit the number of packets specified
		    		ackPayload ack = packets_needed_future.get();
		    		packets_needed = ack.first;
		    		ack_block_ID = ack.second;
	    			if(verb){std::cout << "receive ack for " << (int)ack_block_ID << " with packets_needed " << packets_needed << "\n";}
	    			if(packet_needed_per_block_ID[(int)ack_block_ID] >= N) { // only for the first TX of each block
		    			// I sent N*(1-PER) packets, packets_needed were not received -> estimate PER
		    			PER_estimate = (1-alpha)*(double)packets_needed/std::ceil((packet_needed_per_block_ID[(int)ack_block_ID]/(1 - PER_estimate))) + alpha*PER_estimate;
		    			if(verb) {std::cout << "PER_estimate " << PER_estimate << "\n";}
		    		}
		    		packet_needed_per_block_ID[(int)ack_block_ID] = packets_needed;

		    		// change timeout_span dynamically only when an ACK is received after a tx
		    		auto tx_end = std::chrono::high_resolution_clock::now();
		    	}
		    	th_ACK.join(); // clean up
	    	} while (packet_needed_per_block_ID[(int)block_ID] != 0);
	    	// free buffer
	    	free(input_buffer);
	    	sentPackets += packet_sent_per_block_ID[block_ID];

	    	// the following code only measures the PER, it does not have influence on the number of packet sent
	    	if(no_history) { // not enough packets yet to create an history
	    		int total_sent = 0;
	    		for(int block_index = 0; block_index <= (int)block_ID; ++block_index) {
	    			total_sent += packet_sent_per_block_ID[block_index];
	    		}
	    		PPoverhead = (double)total_sent/(block_ID+1);
	    		if(verb) {std::cout << "Packet per block_ID " << PPoverhead << "\n";}
	    		no_history = (block_ID >= PPO_history) ? 0 : 1;
	    	} else {
	    		int total_sent = 0;
	    		if(block_ID - PPO_history >= 0) {
	    			for(int block_index = block_ID; block_index > (int)block_ID - PPO_history; --block_index) {
	    				//std::cout << "block_index " << block_index << "\n";
	    				total_sent += packet_sent_per_block_ID[block_index];
	    			}
	    		} else {
	    			for(int block_index = block_ID; block_index >= 0; --block_index) {
	    				//std::cout << "block_index " << block_index << "\n";
	    				total_sent += packet_sent_per_block_ID[block_index];
	    			}
	    			for(int block_index = UCHAR_MAX - 1; block_index > UCHAR_MAX + (block_ID - PPO_history); --block_index) {
	    				//std::cout << "block_index " << block_index << "\n";
	    				total_sent += packet_sent_per_block_ID[block_index];
	    			}
	    		}
	    		PPoverhead = (double)total_sent/(PPO_history);
	    		if(verb) {std::cout << "Packet per block_ID " << PPoverhead << "\n";}
	    		if(verb) {std::cout << "Possible PER " << PER_estimate << "\n";}
	    	}
	    }
	}
	else {
		std::cout << "error in reading input file";
		return 2;
	}
	end_file_tx = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds_file_tx = end_file_tx-start_file_tx;

	std::cout << "packet_sent = " << sentPackets <<"\n";
	std::cout << "packet_in_file = " << file_length/PAYLOAD_SIZE + 1 << "\n"; // it does not consider the zero padding of the last block
	std::cout << "elapsed time = " << elapsed_seconds_file_tx.count() << "s\n";
	std::cout << "goodput = " << (double)file_length*8/(elapsed_seconds_file_tx.count()*1000000) << " Mbits\n";
	std::cout << "throughput = " << (double)sentPackets*PAYLOAD_SIZE*8/(elapsed_seconds_file_tx.count()*1000000) << " Mbits\n";

	freeaddrinfo(res_dst);
	// close socket
	close(sockfd_send);
	return 0;
}
