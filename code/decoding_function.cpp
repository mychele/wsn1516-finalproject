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

packetNeededAndVector packet_decoder(std::vector<NCpacket> packetVector)
{

    packetNeededAndVector out;
    std::vector<bitset<K_TB_SIZE>> M;
    int i=0;
    std::vector<char*> encoded_payloads;
    std::vector<char*> decoded_data;
    for(std::vector<NCpacket>::iterator pckIt = packetVector.begin(); pckIt != packetVector.end(); ++pckIt)
    {
        M.push_back(pckIt->getBinaryHeader());
        //cout<<"seed :"<<pckIt->getHeader()<<"\n";
        encoded_payloads.push_back(pckIt->getPayload());
    }

    std::vector<std::vector<unsigned short int> > u(N_TB_SIZE);
    std::vector<std::vector<unsigned short int> > e(N_TB_SIZE);
    //vector of outcoming nodes for each left node, representing and unecoded data pck
    std::vector<std::vector<unsigned short int> > v(K_TB_SIZE);
    std::vector<std::vector<unsigned short int> > d(K_TB_SIZE);
    for (int i=0; i<N_TB_SIZE; i++)
    {
        for (int j=0; j<K_TB_SIZE; j++)
        {
            if (M.at(i)[j]==1)
            {
                u[i].push_back(j);
                v[j].push_back(i);
            }

        }

    }
    for (int j=0; j<K_TB_SIZE; j++)
    {
        if (v[j].size()==0)
        {
            out.first=1;
            return out;
        }

    }
    bool something_done=0;
    unsigned short int pivot;
    //edge: left node (pivot), right node
    std::vector<unsigned short int> pivot_vector;
    int current_node;
    do
    {
        something_done=0;
        for (int i=0; i<N_TB_SIZE; i++)
        {
            //one edge: remove it and note how left node is decoded (vectors d and e)
            if (u[i].size()==1)
            {
                something_done=1;
                pivot=u[i].back();
                pivot_vector.push_back(pivot);
                //aggiungi il nodo stesso
                e[i].push_back(i);
                u[i].pop_back();
                //altrimenti il nodo di sinistra è già stato decodificato in precedenza, e non c'è bisogno di farlo nuovamente
                if (d[pivot].size()==0)
                {
                    for (int k=0; k<e[i].size(); k++)
                        d[pivot].push_back(e[i].at(k));
                }
                //la combinazione di XOR a e[i] viene cancellata e poi riscritta al passaggio successivo (eliminazione degli archi da sinistra a destra).
                //questo non è un problema poiché la combinazione di XOR a sinistra non viene mai aggiornata (viene cioè creata una volta in corrispondenza
                //di una eliminazione da destra a sinistra, e poi rimane sempre uguale
                //il motivo è che così non si deve memorizzare il nodo a destra i al quale punta il nodo pivot a sinistra (altrimenti quando si aggiornano gli
                //XOR a destra verrebbe i XOR i
                e[i].clear();
            }
        }
        while (pivot_vector.size()>0 && something_done)
        {
            pivot=pivot_vector.back();
            //cout<<"pivot is "<<pivot<<endl;
            pivot_vector.pop_back();
            for (int j=v[pivot].size()-1; j>=0; j--)
            {
                //current right node
                current_node=v[pivot].back();
                //aggiorna gli XOR a destra (aggiungi la codifica del nodo pivot)
                for (int k=0; k<d[pivot].size(); k++)
                {
                    e[current_node].push_back(d[pivot].at(k));

                }
                //elimina l'arco (due archi orientati) che collega v[pivot] a v[pivot].at(j)
                v[pivot].pop_back();
                for (int k=0; k<u[current_node].size(); k++)
                {
                    if (u[current_node].at(k)==pivot)
                    {
                        u[current_node].erase(u[current_node].begin()+k);
                        break;
                    }

                }

            }
        }

    }
    while (something_done);
    for (int i=0; i<K_TB_SIZE; i++)
    {
        if (d[i].size()==0)
        {
            out.first=1;
            return out;
        }

    }
    std::vector<bitset<N_TB_SIZE>> M_inv;
    bitset<N_TB_SIZE> tmp;
            for (int j=0; j<N_TB_SIZE; j++)
        {
            tmp[j]=0;

        }
    for (int i=0; i<K_TB_SIZE; i++)
    {
        M_inv.push_back(tmp);
    }
    for (int i=0; i<d.size(); i++)
    {
        for (int j=0; j<d[i].size(); j++)
        {
            M_inv.at(i)[d[i].at(j)]=M_inv.at(i)[d[i].at(j)]^1;

        }
    }
    decoded_data=XOR_decode(M_inv, encoded_payloads);
    out.first=0;
    out.second=decoded_data;
    decoded_data.clear(); //io l'ho lasciato perché funziona lo stesso: evidentemente out.second=decoded_data fa una copia dei dati (mi aspettavo un passaggio del riferimento, ma tant'è...)
    return out;

    }
