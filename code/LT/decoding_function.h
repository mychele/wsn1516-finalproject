#ifndef DECODING_FUNCTION_H
#define DECODING_FUNCTION_H

#include <stdlib.h>
#include "NCpacket.h"
#include <vector>
#include "NCpacketHelper.h"

// if int!=0 vector is void, if int==0, vector contains decoded packets
typedef std::pair<int, std::vector<char*>> packetNeededAndVector ;

/**
* decode the received packets (extract encoding vectors and decode)
* @param vector of packets
* @param object with RSD utils
* @returns number of packets needed to complete decoding and vector of decoded packets
*/

packetNeededAndVector  packet_decoder(std::vector<NCpacket> *packetVector, NCpacketHelper *nchelper);

#endif
