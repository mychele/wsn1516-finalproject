#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <bitset>
#include <stdio.h>
#include <istream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "NCpacket.h"
#include "utils_wsn.h"
#include <cmath>
#include <ctgmath>
#include <vector>
#include <iterator>
#include "decoding_function.h"

packetNeededAndVector packet_decoder(std::vector<NCpacket> *packetVector, NCpacketHelper *nchelper)
{
    packetNeededAndVector out;
    std::vector< std::vector<int> > M;
    M.reserve(packetVector->size());
    int i=0;
    std::vector<char*> encoded_payloads;
    encoded_payloads.reserve(packetVector->size());
    std::vector<char*> decoded_data;
    decoded_data.reserve(packetVector->size());
    for(std::vector<NCpacket>::iterator pckIt = packetVector->begin(); pckIt != packetVector->end(); ++pckIt)
    {
        M.push_back(nchelper->getBinaryHeader(pckIt->getHeader()));
        encoded_payloads.push_back(pckIt->getPayload());
    }
    //each element of u gives the vector of incoming nodes for current right node, i.e. UNCODED pcks that are combined in packet corresponding to the element
    std::vector<std::vector<unsigned short int> > u(N_TB_SIZE);
    //each element of e gives the vector of ENCODED packets whose XOR combination is equivalent the encoded packet correspondng to the element (XOR combinations on the right)
    std::vector<std::vector<unsigned short int> > e(N_TB_SIZE);
    //each element of v gives a vector of outcoming nodes for left node corresponding to the current element, which represents an UNCODED data pck
    std::vector<std::vector<unsigned short int> > v(K_TB_SIZE);
    //each element of d gives the vector of ENCODED packets needed to decode the packet corresponding to the element (XOR combinations on the left)
    std::vector<std::vector<unsigned short int> > d(K_TB_SIZE);
    int degree = 0;
    int pos = 0;
    for (int i = 0; i < N_TB_SIZE; i++) // cycle on N encoding vector
    {
        degree = M.at(i).size();
        for (int j = 0; j < degree; j++) // cycle on an encoding vector
        {
            pos = M.at(i)[j]; // get the position of the 1
            u[i].push_back(pos);
            v[pos].push_back(i);
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
    bool something_done=0; //check whether during an iteration of message passing some node
    unsigned short int pivot;
    //edge: left node (pivot), right node
    std::vector<unsigned short int> pivot_vector;
    int current_node;
    do
    {
        something_done=0;
        for (int i=0; i<N_TB_SIZE; i++)
        {      
            if (u[i].size()==1) //one edge: remove it and note how left node is decoded (vectors d and e)
            {
                something_done=1;
                pivot=u[i].back();
                pivot_vector.push_back(pivot);
                e[i].push_back(i); //add the node itself
                u[i].pop_back(); 
                if (d[pivot].size()==0) //otherwise left node had already been decoded previously, threfore there's no need to do it again
                {
                    for (int k=0; k<e[i].size(); k++)
                        d[pivot].push_back(e[i].at(k));
                }
		/*
		the combination of XOR at e[i] is erased and rewritten at the successive passage 
		(elimination of arcs from left to right). This is not a problem, since the combination of XOR on the left
		is never updated (i.e. it is created once in correspondance of an elimination from right to left, and then remains
		always the same).
		The reason of doing so is that in this way we don't have to memorize the right node i to which node pivot 
		on the left points (otherwise, if we didn't do that, when XOR on the right are updated, we would have i XOR i)
		*/
                e[i].clear();
            }
        }
        while (pivot_vector.size()>0 && something_done)
        {
            pivot=pivot_vector.back();
            pivot_vector.pop_back();
            for (int j=v[pivot].size()-1; j>=0; j--)
            {    
                current_node=v[pivot].back(); //current right node   
                for (int k=0; k<d[pivot].size(); k++) //update XOR on the right (add coding yielded by node pivot)
                {
                    e[current_node].push_back(d[pivot].at(k));

                }
                //detele arc (i.e. two oriented arcs) linking v[pivot] to v[pivot].at(j)
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
    while (something_done); //continue until there is an iteration when nothing is done (i.e. message passing stops)
    
    for (int i=0; i<K_TB_SIZE; i++)
    {
        if (d[i].size()==0) //an uncoded packet has not been resolved by message passing (i.e. no combination of encoded packets that give the uncoded packet has been found)
        {
            out.first=N_TB_SIZE; //the algorithm has failed: return N_TB_SIZE: no decoding is possible
            return out;
        }

    }
    std::vector<bitset<N_TB_SIZE>> M_inv = std::vector<bitset<N_TB_SIZE>>(K_TB_SIZE);   
    for (int i=0; i<d.size(); i++) //according to the XOR combinations found by the message passing algorithm, build the decoding matrix
    {
        for (int j=0; j<d[i].size(); j++)
        {
            M_inv.at(i)[d[i].at(j)]=M_inv.at(i)[d[i].at(j)]^1;

        }
    }
    decoded_data=XOR_decode(M_inv, encoded_payloads); //decode the payloads
    out.first=0; //decoding successful: no more packets are needed for this block
    out.second=decoded_data;
    decoded_data.clear(); 
    return out;

}
