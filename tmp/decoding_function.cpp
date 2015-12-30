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
    int const K=10; //K? // MP: visto che hai incluso NCpacket, puoi usare K_TB_SIZE (32)
    int N=6; //size of packetVector? // MP: packetVector.size();
    const int m=15; //m? // MP: PAYLOAD_SIZE definita in NCpacket
    mat_GF2 data_enc;
    data_enc.SetDims(N,m); // MP: questa poi non la usi più, forse si può togliere?
    mat_GF2 M; // MP: M è la matrice con gli encoding vector? Usare nomi più estesi a volte aiuta il debug!
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
        encoded_payloads.at(i)=pckIt->getPayload();
        i++;
    }

    mat_GF2 M_id;
    M_id=append_identity(M);
    gauss(M_id);
    //index of last nonzero row
    int last_nonzero=N-1;
    int flag_nonzero=0;
    // MP: da qui in poi dovrebbe funzionare, l'avevi testato no?
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

        //write file
        // open file for the first time         
        int DATA_SIZE=decoded_data.size()*sizeof(decoded_data.at(0)); // MP: dovrebbe essere K_TB_SIZE*PAYLOAD_SIZE giusto?
                                // MP: il nome file non è il secondo argomento di input della funzione?
                                // perché argv[1]?
        std::ofstream output_file (argv[1], std::ios::out | std::ios::app | std::ios::binary);
        if (output_file.is_open())
        {
            // MP: ho paura che così si scrive il primo elemento di decoded_data, 
            // che va ad occupare DATA_SIZE, quindi più di quello che dovrebbe occupare
            // Non sono sicuro che gli altri elementi di std::vector siano contigui in 
            // memoria al primo
            // la cosa più sicura secondo me è 
            // for(std::vector<*char>::iterator pckIt = decoded_data.begin(); pckIt != decoded_data.end(); ++pckIt)
            // {
            //     output_file.write(decoded_data.at(0),PAYLOAD_SIZE);
            // }
            // // output_file.write(decoded_data.at(0),DATA_SIZE);
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



