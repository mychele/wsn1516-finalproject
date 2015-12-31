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
#include <NTL/mat_GF2.h>
#include <bitset>
#include <vector>


#ifndef UTILS_WSN
#define UTILS_WSN

using std::vector;
using std::cout;
using namespace NTL;

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


// randomly initialize with binary numbers and existing matrix
void rand_initialize_matrix(mat_GF2& X, int const r, int const c);
// create a randomly initialized matrix
mat_GF2 rand_create_matrix(int const r, int const c);
// randomly creata data (not encoded) matrix
//K=number of packets, m=bits per packet
mat_GF2 rand_create_data(int const K, int const m);
//append identity matrix to an existing matrix
mat_GF2 append_identity(mat_GF2& X);
// given a data matrix of and an encoding matrix, give endcoded data
mat_GF2 encoded_data(mat_GF2& data_matrix,mat_GF2& encoding_matrix);
// given a matrix of encoded data and the pseudo-inverse of the encoding matrix, give dedcoded data
mat_GF2 decoded_data(mat_GF2& _encoded_data_matrix,mat_GF2& inverse_matrix);
// write matrix (the output of the default write functon: cout<<matrix; is unreadable)
// id_append=1 iff there's and identity matrix appended (divide matrix and identity using ||), otherwise 0
void write_matrix(mat_GF2& X, bool id_appended);
// truncates the matrix resulting from Gaussian-Jordan elimination by deleting the left part of the matrix (identity matrix) and the rows exceeding the rank
mat_GF2 pseudo_inverse(mat_GF2& X, int const matrix_rank);

//LITTLE ENDIAN order: successive elements of the matrix are stored into progressively significant bits starting from the least significant
/*ex: matrix
110000101001100
101011100101111
100...
becomes characters
01000011
10011001
00111010
...
i.e. first character is hex_43='C'*/
void binary_to_char(char* output_data, mat_GF2& X);

/**
* encode the packets according to an encoding vector
* @param binary encoding matrix (EV)
* @param input vector of data (i.e. vector of arrays of chars)
* @param output payload (array of chars)
*/
void XOR_encode(mat_GF2& X, vector<char*>& data, char* out_payload);

vector<char*> XOR_decode(mat_GF2& X, vector<char*>& encoded_data);

unsigned int binary_to_unsigned_int(mat_GF2& X);

#endif
//UTILS_WSN
