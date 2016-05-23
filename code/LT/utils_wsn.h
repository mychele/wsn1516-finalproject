/**
 * Class of utilities shared by different classes
 */

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
#include <bitset>
#include <vector>
#include <chrono>
#include <iostream>


#ifndef UTILS_WSN
#define UTILS_WSN

using std::vector;
using std::cout;


/**
 * This functions returns a vector of NCpackets.
 * @param a pointer to the raw data buffer
 * @param the size of the raw data buffer, it should be K*PAYLOAD
 * @return a vector of encoded NCpackets, or an empty vector if size != K*PAYLOAD
 */
extern std::vector<NCpacket> memoryToVector(char *buffer, int size);

/**
 * This function returns a vector of pointer to char arrays of size PAYLOAD_SIZE
 * @param a pointer to the raw data buffer
 * @param the size of the raw data buffer, it should be K*PAYLOAD
 * @return a vector of pointers to char arrays, or an empty vector if size != K*PAYLOAD
 */
extern std::vector<char *> memoryToCharVector(char *buffer, int size);

/**
 * This function converts the vector<char *> given by memoryToCharVector
 * into a std::vector<NCpacket>
 * @param a vector of pointers to char arrays
 * @return a vector of encoded NCpackets
 */
extern std::vector<NCpacket> charVectorToNCVector(std::vector<char *> v);


/**
 * Convert buffer into NCpacket.
 * @param buffer with raw received packet
 * @return a NCpacket
 */
extern NCpacket deserialize(char *buffer);

/**
 * Send all the bytes that have to be sent or return -1
 * @param the socket file descriptor
 * @param a pointer to the data to send
 * @param an int with the amount of byte to send
 * @param addrinfo struct with info on receiver
 * @return 0 if successful, -1 if errors
 */
extern int sendall(int sockfd_send, char *send_buffer, int *byte_to_send, addrinfo *p_iter);

/**
 * Get a pointer to the correct version of the address in sockaddr, IPv4 or IPv6
 * @param a sockaddr structure
 */
extern void *get_in_addr(struct sockaddr *sa);

/**
 * Get the value of the correct version of the port in sockaddr, IPv4 or IPv6
 * @param a sockaddr structure
 * @return the port
 */
extern in_port_t get_in_port(struct sockaddr *sa);

/**
 * from unsigned int to char array
 * @param a char array
 * @param the int to be converted
 */
void packu32(unsigned char *buf, unsigned int i);

/**
 * from char array to unsigned int
 * @param the char buffer to be converted
 * @return a int
 */
unsigned int unpacku32(unsigned char *buf);

/**
 * from std::chrono::nanoseconds to timeval
 * @param a std::chrono::microseconds value
 * @return a timeval structure
 */
timeval timeConversion(std::chrono::microseconds d);

/**
* encode the packets according to an encoding vector
* @param binary encoding matrix (EV)
* @param input decoded vector of data (i.e. vector of arrays of chars)
* @param output encoded payload (array of chars)
*/
void XOR_encode(std::vector<int> *encoding_vector, vector<char*> *data, char* out_payload);

/**
* decode the packets according to an encoding vector
* @param binary encoding matrix (rows are the EVs)
* @param input encoded vector of data (i.e. vector of arrays of chars)
* @return decoded payload (array of chars)
*/
vector<char*> XOR_decode(vector<bitset<N_TB_SIZE>>&X, vector<char*>& encoded_data);

#endif
//UTILS_WSN
