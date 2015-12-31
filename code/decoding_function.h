#ifndef DECODING_FUNCTION_H
#define DECODING_FUNCTION_H

#include <stdlib.h>
#include "NCpacket.h"
#include <vector>

using namespace NTL;

int packet_decoder(std::vector<NCpacket> packetVector, const char* filename);

#endif
