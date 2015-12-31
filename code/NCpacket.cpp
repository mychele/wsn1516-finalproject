#include "NCpacket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>
#include "utils_wsn.h"
#include <bitset>

// TODO: find a safer way to handle the payload array (for example check size!)

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

NCpacket::NCpacket(vector<char*> data, unsigned char block_ID)
{
    // create encoding vector
    mat_GF2 encoding_vector=rand_create_matrix(1,K_TB_SIZE);
    char *tmp=(char *)calloc(PAYLOAD_SIZE,sizeof(char));  //needs to be preallocated
    // create payload
    XOR_encode(encoding_vector, data, tmp);
    // store payload and ev
    memcpy(packet.payload, tmp, PAYLOAD_SIZE); 
    packet.header=binary_to_unsigned_int(encoding_vector);
    packet.block_ID = block_ID;
     //write_matrix(encoding_vector,0);
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

char*
NCpacket::serialize()
{
    char* returnPointer = (char *) calloc(sizeof(NCpacket), sizeof(char));
    packu32((unsigned char*)returnPointer, packet.header);
    *(returnPointer + sizeof(unsigned int)) = packet.block_ID;
    memcpy(returnPointer + sizeof(unsigned int) + sizeof(char), (char*)packet.payload, PAYLOAD_SIZE);
    return returnPointer;
}

mat_GF2
NCpacket::getBinaryHeader()
{
    mat_GF2 binaryHeader;
    binaryHeader.SetDims(1, K_TB_SIZE);
    for(int i = 0; i < K_TB_SIZE; i++)
    {
        unsigned char bit = (unsigned char)((packet.header >> (i)) & 1) + '0';
        binaryHeader[0][i] = bit;
    }
    return binaryHeader;
}

std::ostream& operator<<(std::ostream &strm, const NCpacket &packet_ext)
{
    const char *payload_pointer = packet_ext.getPayload();
    int header_pck = packet_ext.getHeader();
    std::string payload_string(payload_pointer, PAYLOAD_SIZE);
    return strm << "\nNCpacket:\nheader = " << header_pck;// << "\npayload:\n" << payload_string << "\n";
}
