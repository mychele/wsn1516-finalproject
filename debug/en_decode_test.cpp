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
#include "decoding_function.h"

using namespace std;
int main(int argc, const char *argv[])
{
    srand(time(0));
    // read file
    std::ifstream input_file (argv[1], std::ifstream::binary);
    std::cout << "K " << K_TB_SIZE << "\n";
    cout<<argc<<endl;
    cout<<argv<<endl;
    if(input_file)
    {
        // read file size
        // get length of file:
        input_file.seekg (0, input_file.end);
        int file_length = input_file.tellg();
        // set cursor at the beginning
        input_file.seekg (0, input_file.beg);
        cout<<"ok1\n";
        int num_encoding_op = ceil((float)file_length/(K_TB_SIZE*PAYLOAD_SIZE)); // in TB of size K_TB_SIZE*PAYLOAD_SIZE byte
        // the last one may need padding, provided by calling calloc
        for(int encoding_op_index = 0; encoding_op_index < num_encoding_op; encoding_op_index++)
        {
            cout<<"ok1.5\n";
            // create input buffer for K packets
            char *input_buffer; //PAYLOAD_SIZE and K_TB_SIZE are defined in NCpacket.h
            input_buffer = (char *)calloc(PAYLOAD_SIZE*K_TB_SIZE, sizeof(char));
            // read K_TB_SIZE packets of PAYLOAD_SIZE byte
            input_file.read((char *)input_buffer, PAYLOAD_SIZE*K_TB_SIZE);
            std::vector<char *> charPointerVector = memoryToCharVector(input_buffer, PAYLOAD_SIZE*K_TB_SIZE);
            // -------------------------------- encode ----------------------------------------
            // INPUT: charPointerVector, a vector of pointers to char array
            // OUTPUT: a std::vector<NCpacket>
            std::vector<NCpacket> encoded_packets;
            cout<<"ok2\n";
            for (int i=0; i<K_TB_SIZE; i++)
            {
                cout<<"ok3\n";
                NCpacket nc=NCpacket(charPointerVector);
                cout<<"ok4\n";
                // MP: qui crei due volti il pacchetto? è solo per debug?
                encoded_packets.push_back(NCpacket(charPointerVector));
            }
            // free buffer
            free(input_buffer);
            int rem;
            cout<<"ok5\n";
            rem=packet_decoder(encoded_packets, argv[2]);
            cout<<"ok8\n";
            cout<<"outside function: remaining= "<<rem<<endl;
            // -------------------------------- decode ----------------------------------------
            // INPUT: a std::vector<NCpacket>
            // OUTPUT: charPointerVector named nc_vector, a vector of pointers to char array (//TODO discuss on this)


            // save to file


        }
        cout<<"ok10"<<endl;
    }
    else
    {
        std::cout << "error in reading input file";
        return 2;
    }
    /*
    char* p;
    char* t;
    t[0]=-1;
    char sum=0;
    sum=sum ^ t[0];
    p[0]=sum;
    cout<<"result is::"<<(int)p[0]<<"::"<<endl;
    cout<<"everything's fine\n";*/
    cout<<"ok final"<<endl;
    return 0;
}
