/**
 * This is the header file for NCpacket
 */ 

#ifndef NCPACKET
#define NCPACKET

#define K 32
#define PAYLOAD_SIZE 1024 //in byte


struct NCpacketContainer
{
	int header; // 32 bit
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
	 * @param an int with the header
	 * @param a pointer to the payload array
	 */ 
	NCpacket(int header, char* payload);

	/**
	 * Set the header of an NCpacket
	 * @param an int containing the 32 bit header
	 */
	void setHeader(int header);

	/**
	 * Get the header of an NCpacket
	 * @return the header
	 */
	int getHeader();

	/**
	 * Set the payload of an NCpacket
	 * @param a pointer to the char array where the payload is stored
	 */
	void setPayload(char *payload);

	/**
	 * Get the payload of an NCpacket
	 * @return a pointer to the first entry of the payload
	 */
	char* getPayload();	

	/**
	 * Get the size of the payload of an NCpacket
	 * @return the payload size
	 */
	int getPayloadSize();

};

#endif 
//NCPACKET