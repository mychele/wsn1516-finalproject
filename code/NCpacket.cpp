#include "NCpacket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// TODO: find a safer way to handle the payload array (for example check size!)

void
NCpacket(int header, char* payload) {
	NCpacketContainer packet;
	packet.header = header;
	memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

void
NCpacket::setHeader(int header) {
	packet.header = header;
}

int 
NCpacket::getHeader() {
	return packet.header;
}

void
NCpacket::setPayload(char *payload) {
	memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

char*
NCpacket::getPayload() {
	char* returnPointer = (char *) malloc(PAYLOAD_SIZE*sizeof(char));
	memcpy(returnPointer, packet.payload, PAYLOAD_SIZE);
	return returnPointer;
}

int
NCpacket::getPayloadSize() {
	return PAYLOAD_SIZE;
}