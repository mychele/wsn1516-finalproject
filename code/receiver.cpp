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
#include <limits.h>

#define RECEIVER_PORT "30000"
#define BACKLOG 100

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
int sendack(unsigned int packets_needed, unsigned char block_ID, struct sockaddr_storage sender_addr)
{
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
                      &ack_hints, &ack_res)) != 0)
    {
        fprintf(stderr, "ACK getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    // at this point res is a linked structure filled with useful data
    // navigate it!
    int sockfd_ack; // socket file descriptor
    // let's get the socket and bind!
    for(p_iter = ack_res; p_iter != NULL; p_iter = p_iter->ai_next)
    {
        // make a socket for the first valid entry in the linked list
        // also check errors
        if((sockfd_ack = socket(p_iter->ai_family, p_iter->ai_socktype,
                                p_iter->ai_protocol)) == -1)
        {
            // there was an error, try with the next one
            perror("receiver ack: socket");
            continue;
        }
        break;
    }

    if (p_iter == NULL)   //no valid address was found
    {
        std::cout << "receiver: unable to create ACK socket\n";
        return 2;
    }

    // send ack
    int byte_to_send = sizeof(int) + sizeof(char);
    unsigned char *ack_buffer = (unsigned char *)calloc(byte_to_send, sizeof(char));
    packu32(ack_buffer, packets_needed);
    *(ack_buffer + sizeof(int)) = block_ID;
    int byte_sent;
    if((byte_sent = sendall(sockfd_ack, (char*)ack_buffer, &byte_to_send, p_iter)) == -1)
    {
        perror("receiver: ack socket");
        std::cout << "Sent only " << byte_sent << " byte because of an error\n";
    }
    close(sockfd_ack);
    return byte_sent;
}


int main(int argc, char *argv[])
{
    // for testing and simulation
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_real_distribution<double> distr(0.0, 1.0); // define the range
    double PER; // packet error rate
    PER = (argc < 3) ? 0 : atof(argv[2]); // assign argv[2] or 0 if argv[2] not provided

    struct addrinfo hints, *res, *p_iter;
    struct sockaddr_storage sender_addr;
	socklen_t sizeof_sender_addr = sizeof sender_addr;

    int status;
    int FILE_LENGTH;
    if (argc < 2)
    {
        std::cout << "usage: receiver filename (PER) with PER optional, default to 0\n";
        return 2;
    }

    // set the whole hints to 0, then change what should be changed
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_DGRAM;
    // hints.ai_flags = AI_PASSIVE;
    // TODO localhost instead of NULL or this computer by using hints.ai_flags= AI_PASSIVE
    if ((status = getaddrinfo("localhost", RECEIVER_PORT, &hints, &res)) != 0)
    {
        fprintf(stderr, "first getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }
    // at this point res is a linked structure filled with useful data
    // navigate it!
    int sockfd; // socket file descriptor
    // let's get the socket and bind!
    for(p_iter = res; p_iter != NULL; p_iter = p_iter->ai_next)
    {
        // make a socket for the first valid entry in the linked list
        // also check errors
        if((sockfd = socket(p_iter->ai_family, p_iter->ai_socktype,
                            p_iter->ai_protocol)) == -1)
        {
            // there was an error, try with the next one
            perror("receiver: socket");
            continue;
        }

        // bind
        int bind_result;
        if ((bind_result = bind(sockfd, p_iter->ai_addr,
                                p_iter->ai_addrlen)) == -1)
        {
            // there was an error, try with the next one
            perror("receiver: bind");
            continue;
        }
        break;
    }

    if (p_iter == NULL)   //no valid address was found
    {
        std::cout << "receiver: unable to create ACK socket\n";
        return 2;
    }

    std::cout << "Ready to communicate, PER = " << PER << " K_TB_SIZE = " << K_TB_SIZE << "\n";

    std::vector<NCpacket> nc_vector;
    nc_vector.clear();
    int rec_bytes;

    // open file for the first time
    std::ofstream output_file (argv[1], std::ios::out | std::ios::binary);
    if (output_file.is_open())
    {
        output_file.close();
    }

    // stats
    int total_received_packets = 0;
    std::chrono::time_point<std::chrono::system_clock> start_file_rx, end_file_rx_and_decoding, start_packet_decoder, end_packet_decoder;
    double total_time_decoding_block=0;
    int num_blocks=0;

    // useful variables
    unsigned int file_length; // file length in byte
    bool first_block_rx = 1;
    bool first_packet_rx = 1;
    unsigned char rx_block_ID = 0;
    bool file_complete=false;
    // create receive buffer
    void* receive_buffer = malloc(PAYLOAD_SIZE*K_TB_SIZE*sizeof(char));
    // variables for socket select call
    // timeout value
    std::chrono::milliseconds timeout_span(100); // to ultimately avoid deadlock
	struct timeval tv = timeConversion(timeout_span);

    while(!file_complete)
    {
        int received_packets = 0;
        int packets_needed;
        packetNeededAndVector decoded_info;  //pair: first is int containing needed packets; second is vector<char*> with decoded data
        decoded_info.first = K_TB_SIZE;
        std::chrono::time_point<std::chrono::system_clock> start_block_decoding, end_block_decoding;
        start_block_decoding = std::chrono::system_clock::now();  //not sure of the position...
        while (decoded_info.first > 0)
        {
            // ------------------receive or timeout-------------------
            NCpacket packet;
        	int rec_bytes = 0;
        	fd_set readfds;
		    FD_ZERO(&readfds);
		    FD_SET(sockfd, &readfds);
			int select_ret = select(32, &readfds, NULL, NULL, &tv);
			if (select_ret > 0) {
				if(first_packet_rx == 1) {
					first_packet_rx = 0;
					// TODO consider if here is fine, just after the reception of firt packet
					start_file_rx = std::chrono::system_clock::now();  //not sure of the position...
				}
				// TODO update the timeout in a more intelligent way 
		        // socket has pending data to read
		        if((rec_bytes = recvfrom(sockfd, receive_buffer, packet.getInfoSizeNCpacket(), 0,
		                             (struct sockaddr*)&sender_addr, &sizeof_sender_addr))==-1)
			    {
			        // there was an error
			        perror("receiver: recvfrom");
			    }
			    else
			    {
			        packet = deserialize((char *)receive_buffer);
			        if(packet.getBlockID() == rx_block_ID)
                    {
                        // this simulates an error and drops the packets
                        double prob = distr(eng);
                        if (prob >= PER) // TODO consider if this should be moved at if(select > 0) and TEST IT!!
                        {
                            nc_vector.push_back(packet);
                            received_packets++;
                        }
                    }
                    else
                    {
                        // send ack to tell that this blockID was received correctly
                        sendack(0, packet.getBlockID(), sender_addr);
                    }
			    }
		    }
		    else if (select_ret == 0) { // timeout!
            	if(!first_packet_rx) {
	                // a timeout has occurred, no packet was received for too long
	                // and this group of packets was not decoded yet
	                // tell the sender how many packets are needed 
	                std::cout << "No packets for too long, send ACK\n";
	                if(nc_vector.size() < K_TB_SIZE) {
	                	sendack(K_TB_SIZE - nc_vector.size(), rx_block_ID, sender_addr);
	                } else { // packets_needed was surely initialized
						sendack(packets_needed, rx_block_ID, sender_addr);
	                }
            	}
		    } else {
				// do nothing, there was an error
		    }

            //------------------------------
            if(nc_vector.size() >= K_TB_SIZE)
            {
                // try to decode and update packets needed
                std::chrono::time_point<std::chrono::system_clock>
                start_packet_decoder = std::chrono::system_clock::now();
                decoded_info = packet_decoder(nc_vector);
                end_packet_decoder = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds_packet_decoder = end_packet_decoder-start_packet_decoder;
                std::cout<<"Elapsed time to execute decoding function : "<<elapsed_seconds_packet_decoder.count()<<" s\n";
                // if decode is successful, write
                if (decoded_info.first==0)
                {
                    std::vector<char *>::iterator v_iter = decoded_info.second.begin();
                    std::ofstream output_file (argv[1], std::ios::out | std::ios::app | std::ios::binary);
                    if (output_file.is_open())
                    {
                        if(first_block_rx)
                        {
                            char* first_payload = *v_iter;
                            // store file length
                            FILE_LENGTH = unpacku32((unsigned char*) first_payload);
                            file_length=FILE_LENGTH;
                            // write the useful payload
                            output_file.write(first_payload + sizeof(file_length),
                                              PAYLOAD_SIZE - sizeof(file_length));
                            file_length -= PAYLOAD_SIZE - sizeof(file_length);
                            v_iter++;
                            first_block_rx = 0;

                        }
                        for(; v_iter != decoded_info.second.end(); ++v_iter)
                        {
                            if(file_length >= PAYLOAD_SIZE)
                            {
                                output_file.write(*v_iter, PAYLOAD_SIZE);
                                file_length -= PAYLOAD_SIZE;
                            }
                            else if (file_length > 0)     // write last chunck
                            {
                                output_file.write(*v_iter, file_length);
                                file_length = 0;
                            }
                            else
                            {
                                file_complete=true;
                            }
                        }
                        output_file.close();
                    }
                    else
                    {
                        std::cout << "Error in opening output_file";
                        return 2;
                    }

                    decoded_info.second.clear();
                }
                packets_needed = decoded_info.first;
                sendack(decoded_info.first, rx_block_ID, sender_addr);
                rx_block_ID = (decoded_info.first == 0) ? (rx_block_ID = (rx_block_ID+1)%UCHAR_MAX) : rx_block_ID;
            }
        }
        end_block_decoding = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds_block_decoding = end_block_decoding-start_block_decoding;
        std::cout << "Decoded blockID (-1) " << (int)rx_block_ID << "\n";
        std::cout << "Elapsed time to decode blockID " << (int)rx_block_ID << ": "<<elapsed_seconds_block_decoding.count()<<" s\n";
        total_time_decoding_block += elapsed_seconds_block_decoding.count();
        num_blocks++;
        nc_vector.clear();
        total_received_packets += received_packets;
        std::cout << total_received_packets << "\n";
    }
    end_file_rx_and_decoding= std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds_file_rx_decoding = end_file_rx_and_decoding-start_file_rx;
    std::cout << "Average block decoding time over "<<num_blocks<<" blocks : " << (double)total_time_decoding_block/num_blocks << "s \n";
    std::cout << "Elapsed time to rx and decode whole file (of "<<(double)FILE_LENGTH/(1000000)<<" Mbytes) : " << elapsed_seconds_file_rx_decoding.count() << " s \n";
    free(receive_buffer);
    close(sockfd);
    std:cout <<"File successfully received and decoded!! :-)\n";
    return 0;

}
