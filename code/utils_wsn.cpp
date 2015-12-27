#include "NCpacket.h"
#include "utils_wsn.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>



// this function will be extended in order to consider also EV
extern std::vector<NCpacket> memoryToVector(char *buffer, int size) {
	std::vector<NCpacket> toBeReturned;
	// check if size is as it should be
	if (size != K_TB_SIZE*PAYLOAD_SIZE) {
		return toBeReturned;
	}
	//std::cout << "In memoryToVector\n";
	//std::cout << buffer << "\n\n\n";
	for (int i = 0; i < K_TB_SIZE; i++) {
		int header = 0;
		NCpacket packet;//(header, buffer);
		packet.setHeader(header);
		packet.setPayload(buffer);
		toBeReturned.push_back(packet); //insert at the end, so that the first entry will be the first pck
		// std::cout << "The packet just built is\n";
		// std::cout << packet;
		buffer = buffer + PAYLOAD_SIZE; // move pointer
	}
	return toBeReturned;
}

extern NCpacket deserialize(char *buffer) {
	//FIXME this does not work properly
	int header;
	memcpy(buffer, &header, sizeof(int));
	NCpacket packet;
	packet.setHeader(header);
	packet.setPayload(buffer + sizeof(int));
	//std::cout << packet << "\n";
	return packet;
}

