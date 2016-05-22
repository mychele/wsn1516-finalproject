#include "NCpacket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>
#include "utils_wsn.h"
#include <bitset>

NCpacket::NCpacket()
{
    NCpacketContainer packet = NCpacketContainer();
}

NCpacket::NCpacket(unsigned int header, unsigned char block_ID, char* payload)
{
    NCpacketContainer packet = NCpacketContainer();
    packet.header = header;
    packet.block_ID = block_ID;
    memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

NCpacket::NCpacket(int header, unsigned char block_ID, std::vector<int> encoding_vector, vector<char*> *data) {
    packet.header = header;
    char *tmp=(char *)calloc(PAYLOAD_SIZE,sizeof(char));  //needs to be preallocated
    // create payload
    XOR_encode(&encoding_vector, data, tmp);
    // store payload and ev
    memcpy(packet.payload, tmp, PAYLOAD_SIZE);
    packet.block_ID = block_ID;
}

void
NCpacket::setHeader(unsigned int header)
{
    packet.header = header;
}

unsigned int
NCpacket::getHeader() const
{
    return packet.header;
}

void
NCpacket::setPayload(char *payload)
{
    memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

char*
NCpacket::getPayload() const
{
    char* returnPointer = (char *) malloc(PAYLOAD_SIZE*sizeof(char));
    memcpy(returnPointer, packet.payload, PAYLOAD_SIZE);
    return returnPointer;
}

int
NCpacket::getPayloadSize()
{
    return PAYLOAD_SIZE;
}

void
NCpacket::setBlockID (unsigned char block_ID)
{
    packet.block_ID = block_ID;
}

unsigned char
NCpacket::getBlockID () const
{
    return packet.block_ID;
}

unsigned int
NCpacket::getInfoSizeNCpacket() const
{
    return sizeof(packet.payload) + sizeof(packet.header) + sizeof(packet.block_ID);
}

char*
NCpacket::serialize()
{
    // serialize the private variable into a char array, and return the pointer.
    char* returnPointer = (char *) calloc(this->getInfoSizeNCpacket(), sizeof(char));
    packu32((unsigned char*)returnPointer, packet.header);
    *(returnPointer + sizeof(unsigned int)) = packet.block_ID;
    memcpy(returnPointer + sizeof(unsigned int) + sizeof(char), (char*)packet.payload, PAYLOAD_SIZE);
    return returnPointer;
}


std::ostream& operator<<(std::ostream &strm, const NCpacket &packet_ext)
{
    const char *payload_pointer = packet_ext.getPayload();
    int header_pck = packet_ext.getHeader();
    strm << "\nNCpacket:\nheader = " << header_pck; 
    // uncomment to print also the payload, output may be very verbose
    // for(int i = 0; i < PAYLOAD_SIZE; i++) {
    //     strm << (int)payload_pointer[i] << "\n";
    // }
    return strm;
}
