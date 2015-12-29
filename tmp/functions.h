#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include<NTL/mat_GF2.h>
#include <bitset>
#include <vector>

using namespace std;
using namespace NTL;

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
