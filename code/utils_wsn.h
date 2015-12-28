#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "NCpacket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#ifndef UTILS_WSN
#define UTILS_WSN

/**
 * This functions returns a vector of NCpackets, it should be extended to account
 * for EV which are currently set to 0
 */
extern std::vector<NCpacket> memoryToVector(char *buffer, int size);

/**
 * Convert buffer into NCpacket
 */ 
extern NCpacket deserialize(char *buffer);

/**
 * Send all the bytes that have to be sent or return -1
 */
extern int sendall(int sockfd_send, char *send_buffer, int *byte_to_send, addrinfo *p_iter);

/**
 * Get a pointer to the correct version of the address in sockaddr, IPv4 or IPv6 (from beej's guide)
 * @param a sockaddr structure
 */
extern void *get_in_addr(struct sockaddr *sa);

/**
 * Get the value of the correct version of the port in sockaddr, IPv4 or IPv6
 * @param a sockaddr structure
 */
extern in_port_t get_in_port(struct sockaddr *sa);

void packu32(unsigned char *buf, unsigned int i);

unsigned int unpacku32(unsigned char *buf);

#endif 
//UTILS_WSN