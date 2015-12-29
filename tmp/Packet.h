#ifndef PACKET_H
#define PACKET_H


#include<NTL/mat_GF2.h>
#include "functions.h"

using namespace std;
using namespace NTL;

class Packet
{
    mat_GF2 ev;
    mat_GF2 encoded_data;
public:
    //X: data matrix: Kxm where K is the nubmer of encoding packets, and m is the number of data bits per packet
    Packet(mat_GF2 X);
    mat_GF2 get_EV(void);
    mat_GF2 get_Encoded(void);
    mat_GF2 decode(mat_GF2 X);
};
#endif
