//NTL library can be found here: http://www.shoup.net/ntl/

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <NTL/ZZ.h>
#include <NTL/vector.h>
#include <NTL/GF2X.h>
#include <NTL/GF2.h>
#include <bitset>
#include <stdio.h>
#include <iostream>
#include <istream>
#include <fstream>
#include <string.h>
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
using namespace NTL;

int packet_decoder(std::vector<NCpacket> packetVector, const char* filename)
{
    int N=packetVector.size();
    int K=K_TB_SIZE;
    mat_GF2 M;
    M.SetDims(N,K);
    int i=0;
    std::vector<char*> encoded_payloads;
    std::vector<char*> decoded_data;
    for(std::vector<NCpacket>::iterator pckIt = packetVector.begin(); pckIt != packetVector.end(); ++pckIt)
    {
        mat_GF2 tmp_ev;
        tmp_ev=pckIt->getBinaryHeader();
        for (int s=0; s<K; s++)
        {
            M[i][s]=tmp_ev[0][s];
        }
        encoded_payloads.push_back(pckIt->getPayload());
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
    } while (last_nonzero>=0);

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
        mat_GF2 M_inv=pseudo_inverse(M_id, last_nonzero+1);
        decoded_data=XOR_decode(M_inv, encoded_payloads);
        std::ofstream output_file (filename, std::ios::out | std::ios::app | std::ios::binary);
        if (output_file.is_open())
        {
            for(std::vector<char *>::iterator v_iter = decoded_data.begin(); v_iter != decoded_data.end(); ++v_iter)
            {
                output_file.write(*v_iter, PAYLOAD_SIZE);
            }
            output_file.close();
        }
        else
        {
            std::cout << "Error in opening output_file";
            return 2;
        }

    }
    decoded_data.clear();
    return remaining;
}



