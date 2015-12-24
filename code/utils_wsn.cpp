#include "NCpacket.h"
#include "utils_wsn.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

// this function will be extended in order to consider also EV
extern std::vector<NCpacket> memoryToVector(unsigned char *buffer, int size) {
	std::vector<NCpacket> toBeReturned;
	// check if size is as it should be
	if (size != K_TB_SIZE*PAYLOAD_SIZE) {
		return toBeReturned;
	}
	for (int i = 0; i < K_TB_SIZE; i++) {
		NCpacket packet(0, buffer);
		toBeReturned.push_back(packet); //insert at the end, so that the first entry will be the first pck
	}
	return toBeReturned;
}