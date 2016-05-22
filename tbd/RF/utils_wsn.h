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
#include <NTL/mat_GF2.h>
#include <bitset>
#include <vector>
#include <chrono>



#ifndef UTILS_WSN
#define UTILS_WSN

using std::vector;
using std::cout;
using namespace NTL;

static const int N_TB_SIZE = K_TB_SIZE + 5;

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
 * Get a pointer to the correct version of the address in sockaddr, IPv4 or IPv6 (from beej's guide)
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
* randomly initialize an existing matrix
* @param matrix
* @param number of rows
* @param number of columns
* @param value of the random seed
*/

void rand_initialize_matrix(mat_GF2& X, int const r, int const c, int const seed);
/**
* create a matrix and randomly initialize it
* @param number of rows
* @param number of columns
* @param value of the random seed
*/
mat_GF2 rand_create_matrix(int const r, int const c, int const seed);

/**
* create a data (not encoded) matrix and randomly initialize it
* @param number of packets
* @param number of bits per packet
* @param value of the random seed
*/
mat_GF2 rand_create_data(int const K, int const m, int const seed);

/**
* append identity matrix (to the right) to an existing matrix
* @param matrix
*/
mat_GF2 append_identity(mat_GF2& X);

/**
* encode data according to a data matrix and an encoding matrix (collections of EVs)
* @param data matrix
* @param encoding matrix
* @return matrix of encoded data
*/
mat_GF2 encoded_data(mat_GF2& data_matrix,mat_GF2& encoding_matrix);

/**
* decode data according to an encoded data matrix and the pseudo-inverse of the encoding matrix
* @param matrix of encoded data
* @param psedo-inverse (i.e. the biggest full-rank matrix given by Gauss-Jordan elimination)
* @return matrix of dencoded data
*/
mat_GF2 decoded_data(mat_GF2& encoded_data_matrix,mat_GF2& inverse_matrix);

/**
* write matrix to screen. 
* @param matrix to be written
* @param id_append=1 if there's and identity matrix appended (divide matrix and identity using ||), otherwise 0
*/
void write_matrix(mat_GF2& X, bool id_appended);

/**
* truncates the matrix resulting from Gauss-Jordan elimination by deleting the left part of the matrix (identity matrix) and the rows exceeding the rank
* @param matrix resulting as output of Gauss-Jordan elimination (inverse + identity matrix)
* @param rank of the inverse
* @return truncated matrix (inverse)
*/
mat_GF2 pseudo_inverse(mat_GF2& X, int const matrix_rank);

/**
* converts matrix of bits to an array of chars. 
Matrix is read by rows.
The conversion is done with LITTLE ENDIAN order: successive elements of the matrix are stored into progressively significant bits starting from the least significant
ex: matrix
110000101001100
101011100101111
100...
becomes characters
01000011
10011001
00111010
...
i.e. first character is hex_43='C'
* @param output array of chars
* @param matrix of bits
*/
void binary_to_char(char* output_data, mat_GF2& X);

/**
* encode the packets according to an encoding vector
* @param binary encoding matrix (EV)
* @param input vector of data (i.e. vector of arrays of chars)
* @param output payload (array of chars)
*/
void XOR_encode(mat_GF2& X, vector<char*>& data, char* out_payload);

/**
* decode the packets according to an encoding vector
* @param binary encoding matrix (rows are the EVs)
* @param input encoded vector of data (i.e. vector of arrays of chars)
* @return decoded payload (array of chars)
*/
vector<char*> XOR_decode(mat_GF2& X, vector<char*>& encoded_data);

unsigned int binary_to_unsigned_int(mat_GF2& X);

#endif
//UTILS_WSN
