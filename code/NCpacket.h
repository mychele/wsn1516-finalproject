/**
 * This is the header file for NCpacket
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <ostream>
#include <NTL/ZZ.h>
#include <NTL/vector.h>
#include <NTL/GF2X.h>
#include <NTL/GF2.h>
#include <NTL/mat_GF2.h>
#include <bitset>

#ifndef NCPACKET
#define NCPACKET

#define K_TB_SIZE 2500
#define PAYLOAD_SIZE 512 //in byte
#define RSD_CONST_C 0.03
#define RSD_CONST_DELTA 0.5

using std::vector;
using std::bitset;
using namespace NTL;

struct NCpacketContainer
{
	unsigned int header; // 32 bit
	unsigned char block_ID; // 8 bit
 	char payload[PAYLOAD_SIZE]; //1024 byte payload
};

/**
 * An NCpacket contains as a payload the encoded packet and as header
 * the encoding vector EV
 */
class NCpacket {

	NCpacketContainer packet; // private variable

public:

	NCpacket();

    /**
	 * Public constructor
	 * @param a vector of pointers to char arrays (raw data)
	 */
    NCpacket(vector<char*> data, unsigned char block_ID);

	/**
	 * Public constructor
	 * @param an int with the header
	 * @param a pointer to the payload array
	 */
	NCpacket(unsigned int header, unsigned char block_ID, char* payload);

	/**
	 * Set the header of an NCpacket
	 * @param an int containing the 32 bit header
	 */
	void setHeader(unsigned int header);

	/**
	 * Get the header of an NCpacket
	 * @return the header
	 */
	unsigned int getHeader() const;

	/**
	 * Set the payload of an NCpacket
	 * @param a pointer to the char array where the payload is stored
	 */
	void setPayload (char *payload);

	/**
	 * Get the payload of an NCpacket
	 * @return a pointer to the first entry of the payload
	 */
	char* getPayload() const;

	/**
	 * Get the size of the payload of an NCpacket
	 * @return the payload size
	 */
	int getPayloadSize();

	/**
	 * Get the size of the useful information to be transmitted
	 * It may be different from sizeof(NCpacket) because of
	 * the host using memory in chunks of 4 byte
	 * @return the size of the information to be transmitted
	 */
	unsigned int getInfoSizeNCpacket() const;

	/**
	 * Set the block_ID of an NCpacket
	 * @param an unsigned char with block_ID
	 */
	void setBlockID (unsigned char block_ID);

	/**
	 * Get the block_ID of an NCpacket
	 * @return an unsigned char with block_ID
	 */
	unsigned char getBlockID () const;

	/**
	 * Get a serialized version of the object
	 * @return a pointer to a char array
	 */
	char* serialize();

	/**
	 * Get binary version of header
	 * @return mat_GF2 with binary representation of header
	 */
    std::bitset<K_TB_SIZE> getBinaryHeader();

	/**
	 * Overload << operator
	 */
	friend std::ostream& operator<<(std::ostream&, const NCpacket&);

};

#endif
//NCPACKET
