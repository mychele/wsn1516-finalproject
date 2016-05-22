#include "NCpacket.h"
#include "utils_wsn.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <bitset>
#include <chrono>
#include <random>
#include <algorithm>

// this function will be extended in order to consider also EV
extern std::vector<NCpacket> memoryToVector(char *buffer, int size)
{
    std::vector<NCpacket> toBeReturned;
    // check if size is as it should be
    if (size != K_TB_SIZE*PAYLOAD_SIZE)
    {
        return toBeReturned;
    }

    for (int i = 0; i < K_TB_SIZE; i++)
    {
        unsigned int header = 50000+1;
        NCpacket packet;//(header, buffer);
        packet.setHeader(header);
        packet.setPayload(buffer);
        toBeReturned.push_back(packet); //insert at the end, so that the first entry will be the first pck
        buffer = buffer + PAYLOAD_SIZE; // move pointer
    }
    return toBeReturned;
}

extern std::vector<char *> memoryToCharVector(char *buffer, int size)
{
    std::vector<char *> toBeReturned;
    // check if size is as it should be
    if (size != K_TB_SIZE*PAYLOAD_SIZE)
    {
        return toBeReturned;
    }

    for (int i = 0; i < K_TB_SIZE; i++)
    {
        toBeReturned.push_back(buffer); //insert at the end, so that the first entry will be the first pck
        buffer = buffer + PAYLOAD_SIZE; // move pointer
    }
    return toBeReturned;
}

extern std::vector<NCpacket> charVectorToNCVector(std::vector<char *> v)
{
    std::vector<NCpacket> toBeReturned;
    for (std::vector<char*>::iterator i = v.begin(); i != v.end(); ++i)
    {
        unsigned int header = 50000+1;
        NCpacket packet;//(header, buffer);
        packet.setHeader(header);
        packet.setPayload(*i);
        toBeReturned.push_back(packet); //insert at the end, so that the first entry will be the first pck
    }
    return toBeReturned;
}

extern NCpacket deserialize(char *buffer)
{
    unsigned int header = unpacku32((unsigned char*)buffer);
    NCpacket packet;
    packet.setHeader(header);
    packet.setBlockID(*(buffer + sizeof(int)));
    packet.setPayload(buffer + sizeof(int) + sizeof(char));
    return packet;
}

extern int sendall(int sockfd_send, char *send_buffer, int *byte_to_send, addrinfo *p_iter)
{
    int total_byte_sent = 0;
    int byte_sent = 0;
    int byte_left = *byte_to_send;

    // send data, possibly in one chunk
    while (total_byte_sent < *byte_to_send)
    {
        byte_sent = sendto(sockfd_send, send_buffer+total_byte_sent,
                           byte_left, 0, p_iter->ai_addr, p_iter->ai_addrlen);
        if(byte_sent == -1)
        {
            break;
        }
        total_byte_sent += byte_sent;
        byte_left -= byte_sent;
    }

    *byte_to_send = total_byte_sent;
    return byte_sent==-1? -1:0; // return failure or success
}

// get sockaddr, IPv4 or IPv6: (from beej's netC++ guide)
extern void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// get inport, IPv4 or IPv6: (from beej's netC++ guide)
extern in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return (((struct sockaddr_in*)sa)->sin_port);
    }
    return (((struct sockaddr_in6*)sa)->sin6_port);
}

void packu32(unsigned char *buf, unsigned int i)
{
    unsigned int network_order_i = htonl(i);
    *buf++ = network_order_i >> 24;
    *buf++ = network_order_i >> 16;
    *buf++ = network_order_i >> 8;
    *buf++ = network_order_i;
}

unsigned int unpacku32(unsigned char *buf)
{
    unsigned int i = ((unsigned  int)buf[0]<<24) |
                     ((unsigned  int)buf[1]<<16) |
                     ((unsigned  int)buf[2]<<8)  |
                     buf[3];
    return ntohl(i);
}

struct timeval timeConversion(std::chrono::microseconds d)
{
    struct timeval tv;
    std::chrono::microseconds usec = std::chrono::duration_cast<std::chrono::microseconds>(d);
    if( usec <= std::chrono::microseconds(0) ) 
    {
        tv.tv_sec = tv.tv_usec = 0;
    }
    else
    {
        tv.tv_sec = usec.count()/1000000;
        tv.tv_usec = usec.count()%1000000;
    }
    return tv;
}


void XOR_encode(std::vector<int> *encoding_vector, vector<char*> *data, char* out_payload) 
{
    vector<char> sum = vector<char>(PAYLOAD_SIZE, 0);
    for (int i=0; i < encoding_vector->size(); i++) // encoding vector contains the position of 1, i.e. which packets must be XORed
    {
        char* pyl=data->at(encoding_vector->at(i)); //get the packets (array of chars) at position given by the EV
        for (int j=0; j<PAYLOAD_SIZE; j++)
        {
            sum[j]=sum[j]^pyl[j];	//perform bitwise XOR on the chars of the packet		

        }
    }
    memcpy(out_payload, &sum[0], PAYLOAD_SIZE); //copy output in output variable
}

vector<char*> XOR_decode(vector<bitset<N_TB_SIZE>>&X, vector<char*>& encoded_data)
{
    vector<char*> out;
    vector<char> sum(PAYLOAD_SIZE);
    for (int i=0; i<K_TB_SIZE; i++) //rows of X are encoding vectors
    {
        out.push_back((char *)calloc(PAYLOAD_SIZE, sizeof(char)));  //allocate memory for output
        std::fill(sum.begin(), sum.end(), 0);
        for (int j=0; j<N_TB_SIZE; j++)	
        {

            if (X.at(i)[j]==1)  //1 at position j of encoding vector i
            {
                char* pyl=encoded_data.at(j); //get packet j
                for (int h=0; h<PAYLOAD_SIZE; h++)
                {
                    sum[h]=sum[h]^pyl[h];     //perform bitwise XOR


                }

            }

        }
        memcpy(out.at(i), &sum[0], PAYLOAD_SIZE); //copy output in output variable
    }
    return out;
}

