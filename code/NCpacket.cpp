#include "NCpacket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

// TODO: find a safer way to handle the payload array (for example check size!)

NCpacket::NCpacket(int header, unsigned char* payload) {
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
NCpacket::setPayload(unsigned char *payload) {
	memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

unsigned char*
NCpacket::getPayload() {
	unsigned char* returnPointer = (unsigned char *) malloc(PAYLOAD_SIZE*sizeof(char));
	memcpy(returnPointer, packet.payload, PAYLOAD_SIZE);
	return returnPointer;
}

int
NCpacket::getPayloadSize() {
	return PAYLOAD_SIZE;
}

unsigned char*
NCpacket::serialize() {
	unsigned char* returnPointer = (unsigned char *) malloc(PAYLOAD_SIZE*sizeof(char) + sizeof(int));
	memcpy(returnPointer, (char*)&packet.header, sizeof(int));
	memcpy(returnPointer + sizeof(int), packet.payload, PAYLOAD_SIZE);
	return returnPointer;
}