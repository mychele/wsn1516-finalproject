#ifndef DECODING_FUNCTION_H
#define DECODING_FUNCTION_H

#include <stdlib.h>
#include "NCpacket.h"
#include <vector>
#include "NCpacketHelper.h"


typedef std::pair<int, std::vector<char*>> packetNeededAndVector ;

packetNeededAndVector  packet_decoder(std::vector<NCpacket> *packetVector, NCpacketHelper *nchelper);

#endif