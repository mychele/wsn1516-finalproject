#ifndef DECODING_FUNCTION_H
#define DECODING_FUNCTION_H

#include <stdlib.h>
#include "NCpacket.h"
#include <vector>

using namespace NTL;

// if int!=0 vector is void, if int==0, vector contains decoded packets
typedef std::pair<int, std::vector<char*>> packetNeededAndVector ;

/**
* decode the received packets (extract encoding vectors and decode). It relies on some methods 
* of utils_wsn
* @param vector of packets
* @returns number of packets needed to complete decoding and vector of decoded packets
*/

packetNeededAndVector  packet_decoder(std::vector<NCpacket> packetVector);

#endif
