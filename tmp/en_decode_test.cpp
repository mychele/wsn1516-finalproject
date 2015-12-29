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


int main(int argc, char const *argv[])
{
	// read file
	std::ifstream input_file (argv[3], std::ifstream::binary);
	std::cout << "K " << K_TB_SIZE << "\n";
	if(input_file) {
		// read file size
		// get length of file:
	    input_file.seekg (0, input_file.end);
	    int file_length = input_file.tellg();
	    // set cursor at the beginning
	    input_file.seekg (0, input_file.beg);

	    int num_encoding_op = ceil((float)file_length/(K_TB_SIZE*PAYLOAD_SIZE)); // in TB of size K_TB_SIZE*PAYLOAD_SIZE byte
	    								// the last one may need padding, provided by calling calloc
	    for(int encoding_op_index = 0; encoding_op_index < num_encoding_op; encoding_op_index++) { 
	    	// create input buffer for K packets
			char *input_buffer; //PAYLOAD_SIZE and K_TB_SIZE are defined in NCpacket.h
			input_buffer = (char *)calloc(PAYLOAD_SIZE*K_TB_SIZE, sizeof(char));
	    	// read K_TB_SIZE packets of PAYLOAD_SIZE byte
	    	input_file.read((char *)input_buffer, PAYLOAD_SIZE*K_TB_SIZE);
	    	std::vector<char *> charPointerVector = memoryToCharVector(input_buffer, PAYLOAD_SIZE*K_TB_SIZE);
	    	// -------------------------------- encode ----------------------------------------
	    	// INPUT: charPointerVector, a vector of pointers to char array
	    	// OUTPUT: a std::vector<NCpacket>
	    	
	    	// free buffer
	    	free(input_buffer);

	    	// -------------------------------- decode ----------------------------------------
	    	// INPUT: a std::vector<NCpacket>
	    	// OUTPUT: charPointerVector, a vector of pointers to char array (//TODO discuss on this)


	    	// save to file
	    	std::ofstream output_file (argv[1], std::ios::out | std::ios::app | std::ios::binary);
			if (output_file.is_open()) {
				for(std::vector<NCpacket>::iterator v_iter = nc_vector.begin(); v_iter != nc_vector.end(); ++v_iter) {
					output_file.write((char *)v_iter->getPayload(), PAYLOAD_SIZE);
				}
				output_file.close();
				nc_vector.clear();
			}
			else {
				std::cout << "Error in opening output_file";
				return 2;
			}

	    }
	}
	else {
		std::cout << "error in reading input file";
		return 2;
	}
}