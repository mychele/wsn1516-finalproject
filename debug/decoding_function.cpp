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


int packet_decoder(std::vector<NCpacket> packetVector, const char* filename)
{
    cout<<"ok5.1\n";
    int const K=32; //K?
    int N=6; //size of packetVector?
    N=K;
    const int m=15; //m?
    mat_GF2 M;
    M.SetDims(32,32);
    int i=0;
    std::vector<char*> encoded_payloads;
    std::vector<char*> decoded_data;
    cout<<"ok5.2\n";
    for(std::vector<NCpacket>::iterator pckIt = packetVector.begin(); pckIt != packetVector.end(); ++pckIt)
    {
        mat_GF2 tmp_ev;
        tmp_ev=pckIt->getBinaryHeader();
        for (int s=0; s<K; s++)
        {
            M[i][s]=tmp_ev[0][s];
            cout<<"i= "<<i<<"; s= "<<s<<endl;
        }
        cout<<pckIt->getPayload()<<endl;
        cout<<"payload size: "<<sizeof(pckIt->getPayload())<<endl;
        encoded_payloads.push_back(pckIt->getPayload());
        i++;
    }
    cout<<"ok5.21\n";
    mat_GF2 M_id;
    M_id=append_identity(M);
    gauss(M_id);
    write_matrix(M_id,1);
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
    cout<<"ok5.3\n";
    int remaining=K-1-last_nonzero;
    if (remaining==0)
    {
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

        }
        cout<<"ok7.1\n";
        mat_GF2 M_inv=pseudo_inverse(M_id, last_nonzero+1);
        cout<<"ok7.2\n";
        decoded_data=XOR_decode(M_inv, encoded_payloads);
        cout<<"ok7.3\n";
        //write file
        // open file for the first time
        int DATA_SIZE=decoded_data.size()*sizeof(decoded_data.at(0));
        cout<<"ok7.4\n";
        std::ofstream output_file (filename, std::ios::out | std::ios::app | std::ios::binary);
        cout<<"ok7.5\n";
        if (output_file.is_open())
        {
            cout<<"ok7.6\n";
            cout<<"size of element of decoded data: "<<sizeof(decoded_data.at(0))<<"; nb of elts: "<<decoded_data.size()<<" => data size: "<<DATA_SIZE<<endl;
            int y=0;
            for(std::vector<char *>::iterator v_iter = decoded_data.begin(); v_iter != decoded_data.end(); ++v_iter)
            {
                output_file.write(*v_iter, DATA_SIZE);
                cout<<"iteration: "<<y<<endl;
                y++;
            }
            cout<<"ok7.65\n";
            output_file.close();
            decoded_data.clear();
            cout<<"ok7.66\n";
        }
        else
        {
            std::cout << "Error in opening output_file";
            return 2;
        }

    }
    cout<<"ok7.7\n";
    cout<<"inside function: remaining= "<<remaining<<endl;
    return remaining;
}



