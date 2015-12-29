#include "NCpacket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>
#include "utils_wsn.h"
#include <bitset>
#include "functions.h"
// TODO: find a safer way to handle the payload array (for example check size!)

NCpacket::NCpacket()
{
    NCpacketContainer packet = NCpacketContainer();
}

NCpacket::NCpacket(unsigned int header, char* payload)
{
    NCpacketContainer packet = NCpacketContainer();
    packet.header = header;
    memcpy(packet.payload, payload, PAYLOAD_SIZE);
}

NCpacket::NCpacket(vector<char*> data)
{
    cout<<"ok3.1\n";
    mat_GF2 encoding_vector=rand_create_matrix(1,K_TB_SIZE);
    cout<<"ok3.2\n";
    char *tmp=(char*)malloc(sizeof(char));   //needs to be preallocated
    XOR_encode(encoding_vector, data, tmp);
    cout<<"size of tmp:"<<sizeof(tmp)<<", sizeof char "<<sizeof(char)<<endl;
    cout<<"payload size "<<PAYLOAD_SIZE<<endl;
    cout<<"ok3.3\n";
    memcpy(packet.payload, tmp, sizeof(tmp)); //size: sizeof(temp) instead of PAYLOAD_SIZE
    cout<<"ok3.4\n";
    packet.header=binary_to_unsigned_int(encoding_vector);
}


// useless if encode/decode with chars

NCpacket::NCpacket(mat_GF2& binary_data)
{
    mat_GF2 encoding_vector=rand_create_matrix(1,K_TB_SIZE);
    mat_GF2 binary_payload=encoding_vector*binary_data;
    char *p;
    binary_to_char(p,binary_data);
    NCpacketContainer packet = NCpacketContainer();
    packet.header = binary_to_unsigned_int(encoding_vector);
    memcpy(packet.payload, p, PAYLOAD_SIZE);
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

char*
NCpacket::serialize()
{
    char* returnPointer = (char *) calloc(sizeof(NCpacket), sizeof(char));
    packu32((unsigned char*)returnPointer, packet.header);
    memcpy(returnPointer + sizeof(unsigned int), (char*)packet.payload, PAYLOAD_SIZE);
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
