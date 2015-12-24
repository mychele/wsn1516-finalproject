#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "NCpacket.h"


#ifndef UTILS_WSN
#define UTILS_WSN

/**
 * This functions returns a vector of NCpackets, it should be extended to account
 * for EV which are currently set to 0
 */
extern std::vector<NCpacket> memoryToVector(unsigned char *buffer, int size);

#endif 
//UTILS_WSN