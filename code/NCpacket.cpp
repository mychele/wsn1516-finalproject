#include "NCpacket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>

// TODO: find a safer way to handle the payload array (for example check size!)

NCpacket::NCpacket() {
	NCpacketContainer packet = NCpacketContainer();
}

NCpacket::NCpacket(int header, char* payload) {
	NCpacketContainer packet = NCpacketContainer();
	packet.header = header;
	memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

void
NCpacket::setHeader(int header) {
	packet.header = header;
}

int 
NCpacket::getHeader() const {
	return packet.header;
}

void
NCpacket::setPayload(char *payload) {
	memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

char*
NCpacket::getPayload() const {
	char* returnPointer = (char *) malloc(PAYLOAD_SIZE*sizeof(char));
	memcpy(returnPointer, packet.payload, PAYLOAD_SIZE);
	return returnPointer;
}

int
NCpacket::getPayloadSize() {
	return PAYLOAD_SIZE;
}

char*
NCpacket::serialize() {
	char* returnPointer = (char *) malloc(PAYLOAD_SIZE*sizeof(char) + sizeof(int));
	memcpy(returnPointer, (char*)&packet.header, sizeof(int));
	memcpy(returnPointer + sizeof(int), packet.payload, PAYLOAD_SIZE);
	return returnPointer;
}

std::ostream& operator<<(std::ostream &strm, const NCpacket &packet_ext) {
	const char *payload_pointer = packet_ext.getPayload();
	int header_pck = packet_ext.getHeader();
	std::string payload_string(payload_pointer, PAYLOAD_SIZE);
  	return strm << "NCpacket:\nheader = " << header_pck << "\npayload:\n" << payload_string << "\n";
}