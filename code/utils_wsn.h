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
 * This functions returns a vector of NCpackets, it should be extended to account
 * for EV which are currently set to 0
 */
extern std::vector<NCpacket> memoryToVector(char *buffer, int size);

/**
 * This function returns a vector of pointer to char arrays of size (hopefully) PAYLOAD_SIZE
 */
extern std::vector<char *> memoryToCharVector(char *buffer, int size);

/**
 * This function converts the vector<char *> given by memoryToCharVector
 * into a std::vector<NCpacket>
 */
extern std::vector<NCpacket> charVectorToNCVector(std::vector<char *> v);


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

/**
 * from unsigned int to char array
 */
void packu32(unsigned char *buf, unsigned int i);

/**
 * from char array to unsigned int
 */
unsigned int unpacku32(unsigned char *buf);

/**
 * from std::chrono::nanoseconds to timeval
 */
timeval timeConversion(std::chrono::microseconds d);

//PMD of Robust Soliton Distribution
std::vector<double> Robust_Soliton_Distribution(int K, double c, double delta);

//CDF with initial 0
std::vector<double> Robust_Soliton_Distribution_CDF(int K, double c, double delta);

//generate a random degree according to a given RSD CDF, starting from a random_number in [0,1]
int random_degree(std::vector<double>* RSD_CDF, double random_number);

// randomly initialize with binary numbers and existing matrix according to row degrees given by RSD
// void rand_initialize_sparse_matrix(std::bitset<K_TB_SIZE>& X, int const c, int const seed,  double const C, double const delta);
// create a randomly initialized sparse matrix according to RSD
std::vector<int> rand_create_sparse_matrix(int const c, int const seed,  double const C, double const delta);

/**
* encode the packets according to an encoding vector
* @param binary encoding matrix (EV)
* @param input vector of data (i.e. vector of arrays of chars)
* @param output payload (array of chars)
*/
void XOR_encode(std::vector<int> *encoding_vector, vector<char*>& data, char* out_payload);

vector<char*> XOR_decode(vector<bitset<N_TB_SIZE>>&X, vector<char*>& encoded_data);

#endif
//UTILS_WSN
