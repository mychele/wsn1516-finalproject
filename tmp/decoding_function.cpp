//http://www.shoup.net/ntl/

#include <iostream>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<NTL/ZZ.h>
#include<NTL/vector.h>
#include<NTL/GF2X.h>
#include<NTL/GF2.h>
#include "Packet.h"
#include "functions.h"
#include <bitset>
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
using namespace NTL;


int packet_decoder(std::vector<NCpacket> packetVector, char *argv[])
{
    int const K=10; //K?
    int N=6; //size of packetVector?
    const int m=15; //m?
    mat_GF2 data_enc;
    data_enc.SetDims(N,m);
    mat_GF2 M;
    M.SetDims(N,K);
    int i=0;
    for(std::vector<NCpacket>::iterator pckIt = packetVector.begin(); pckIt != packetVector.end(); ++pckIt)
    {
        mat_GF2 tmp_ev, tmp_encoded;
        tmp_ev=pckIt->getBinaryHeader();
        tmp_encoded=pckIt->getBinaryPayload();
        for (int s=0; s<K; s++)
        {
            M[i][s]=tmp_ev[0][s];
        }
        for (int s=0; s<m; s++)
        {
            data_enc[i][s]=tmp_encoded[0][s];

        }
        i++;
    }

    mat_GF2 M_id;
    M_id=append_identity(M);
    gauss(M_id);
    //index of last nonzero row
    int last_nonzero=N-1;
    int flag_nonzero=0;
    do
    {
        flag_nonzero=0;
        for (int j=0; j<K; j++)
        {
            if (M_id[last_nonzero][j]!=0)
                flag_nonzero=1;
        }
        if (flag_nonzero==0)
            last_nonzero--;
        else break;
    }
    while (last_nonzero>=0);
    int remaining=K-1-last_nonzero;
    if (remaining==0)

    {
        mat_GF2 data_dec;
        for (int j=K-1; j>=0; j--)
        {
            for (int i=j-1; i>=0; i--)
            {
                if (M_id[i][j]==1)
                {
                    for (int s=0; s<M_id.NumCols(); s++)
                    {
                        M_id[i][s]=M_id[i][s]+M_id[j][s];
                    }

                }

            }
            mat_GF2 M_inv=pseudo_inverse(M_id, last_nonzero+1);
            data_dec=decoded_data(data_enc,M_inv);

        }
        //convert to char
        char* payload;
        std::bitset<8> bits;
        bits.reset();
        int DATA_SIZE=sizeof(data_dec[0][0])*data_dec.NumCols()*data_dec.NumRows();//?? or: int DATA_SIZE=8*data_dec.NumCols()*data_dec.NumRows();
        payload=(char*)malloc(DATA_SIZE);
        int s=0;
        int q=0;
        for (int i=0; i<data_dec.NumRows(); i++)
        {
            for (int j=0; j<data_dec.NumCols(); j++)
            {
                if (data_dec[i][j]==0)
                    bits[s]=0;
                else
                    bits[s]=1;
                s++;
                if (s%8==0)
                {
                    payload[q]=static_cast<char>(bits.to_ulong());
                    bits.reset();
                    s=0;
                    q++;

                }
            }
        }
        if (s!=0)
            payload[q]=static_cast<char>(bits.to_ulong());

        //write file
        // open file for the first time
        std::ofstream output_file (argv[1], std::ios::out | std::ios::app | std::ios::binary);
        if (output_file.is_open())
        {
            output_file.write(payload, DATA_SIZE);
            output_file.close();
        }
        else
        {
            std::cout << "Error in opening output_file";
            return 2;
        }



    }
    return remaining;
}



