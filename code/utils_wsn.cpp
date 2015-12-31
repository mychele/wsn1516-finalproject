#include "NCpacket.h"
#include "utils_wsn.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <bitset>


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
		unsigned int header = 50000+1;
		NCpacket packet;//(header, buffer);
		packet.setHeader(header);
		packet.setPayload(buffer);
		toBeReturned.push_back(packet); //insert at the end, so that the first entry will be the first pck
		buffer = buffer + PAYLOAD_SIZE; // move pointer
	}
	return toBeReturned;
}

extern std::vector<char *> memoryToCharVector(char *buffer, int size) {
	std::vector<char *> toBeReturned;
	// check if size is as it should be
	if (size != K_TB_SIZE*PAYLOAD_SIZE) {
		return toBeReturned;
	}
	//std::cout << "In memoryToVector\n";
	//std::cout << buffer << "\n\n\n";
	for (int i = 0; i < K_TB_SIZE; i++) {
		toBeReturned.push_back(buffer); //insert at the end, so that the first entry will be the first pck
		buffer = buffer + PAYLOAD_SIZE; // move pointer
	}
	return toBeReturned;
}

extern std::vector<NCpacket> charVectorToNCVector(std::vector<char *> v) {
	std::vector<NCpacket> toBeReturned;
	for (std::vector<char*>::iterator i = v.begin(); i != v.end(); ++i) {
		unsigned int header = 50000+1;
		NCpacket packet;//(header, buffer);
		packet.setHeader(header);
		packet.setPayload(*i);
		toBeReturned.push_back(packet); //insert at the end, so that the first entry will be the first pck
	}
	return toBeReturned;
}

extern NCpacket deserialize(char *buffer) {
	unsigned int header = unpacku32((unsigned char*)buffer);
	NCpacket packet;
	packet.setHeader(header);
	packet.setBlockID(*(buffer + sizeof(int)));
	packet.setPayload(buffer + sizeof(int) + sizeof(char));
	//std::cout << packet << "\n";
	return packet;
}

extern int sendall(int sockfd_send, char *send_buffer, int *byte_to_send, addrinfo *p_iter) {

	int total_byte_sent = 0;
	int byte_sent = 0;
	int byte_left = *byte_to_send;

	while (total_byte_sent < *byte_to_send) {
		byte_sent = sendto(sockfd_send, send_buffer+total_byte_sent,
			byte_left, 0, p_iter->ai_addr, p_iter->ai_addrlen);
		if(byte_sent == -1) {
			break;
		}
		total_byte_sent += byte_sent;
		byte_left -= byte_sent;
	}

	*byte_to_send = total_byte_sent;
	return byte_sent==-1? -1:0; // return failure or success
}

// get sockaddr, IPv4 or IPv6: (from beej's guide)
extern void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// get inport, IPv4 or IPv6:
extern in_port_t get_in_port(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }
    return (((struct sockaddr_in6*)sa)->sin6_port);
}

void packu32(unsigned char *buf, unsigned int i) {
	unsigned int network_order_i = htonl(i);
	*buf++ = network_order_i >> 24;
	*buf++ = network_order_i >> 16;
	*buf++ = network_order_i >> 8;
	*buf++ = network_order_i;
}

unsigned int unpacku32(unsigned char *buf)
{
	unsigned int i = ((unsigned  int)buf[0]<<24) |
	                       ((unsigned  int)buf[1]<<16) |
	                       ((unsigned  int)buf[2]<<8)  |
	                       buf[3];
	return ntohl(i);
}

void rand_initialize_matrix(mat_GF2& X, int const r, int const c)
{
    char n;
    for (int i=0; i<r; i++)
    {
        for (int j=0; j<c; j++)
        {
            n=rand()%2;
            X[i][j]=n;
        }
    }

}
mat_GF2 rand_create_matrix(int const r, int const c)
{
    mat_GF2 out_matrix;
    out_matrix.SetDims(r,c);
    rand_initialize_matrix(out_matrix,r,c);
    return out_matrix;
}

mat_GF2 rand_create_data(int const K, int const m)
{
    return rand_create_matrix(K,m);
}

mat_GF2 append_identity(mat_GF2& X)
{
    mat_GF2 out_matrix;
    long const rows=X.NumRows(), columns=X.NumCols();
    long const columns_incr=columns+rows;
    out_matrix.SetDims(rows,columns_incr);
    for (int i=0; i<rows; i++)
    {
        for (int j=0; j<columns; j++)
        {
            out_matrix[i][j]=X[i][j];
        }
    }
    mat_GF2 identity;
    GF2 number;
    number=1;
    diag(identity,rows,number);
    for (int i=0; i<rows; i++)
    {
        for (int j=columns; j<columns_incr; j++)
        {
            out_matrix[i][j]=identity[i][j-columns];
        }
    }

    return out_matrix;
}

void write_matrix(mat_GF2& X, bool id_appended)
{
    long const r=X.NumRows(), c=X.NumCols();

    long const incr=r;

    if (id_appended)
    {
        for (int i=0; i<r; i++)
        {
            for (int j=0; j<c-incr; j++)
            {
                cout<<X[i][j]<<" ";
            }
            cout<<"||"<<" ";
            for (int j=c-incr; j<c; j++)
            {
                cout<<X[i][j]<<" ";
            }
            cout<<"\n";
        }
    }
    else
    {
        for (int i=0; i<r; i++)
        {
            for (int j=0; j<c; j++)
            {
                cout<<X[i][j]<<" ";
            }
            cout<<"\n";
        }
    }
}

mat_GF2 pseudo_inverse(mat_GF2& X, int const matrix_rank)
{
    mat_GF2 out_matrix;
    long const r=X.NumRows(), c=X.NumCols();
    out_matrix.SetDims(matrix_rank,c-matrix_rank);
    for (int i=0; i<matrix_rank; i++)
    {
        for (int j=matrix_rank; j<c; j++)
        {
            out_matrix[i][j-matrix_rank]=X[i][j];

        }
    }
    return out_matrix;
}

mat_GF2 encoded_data(mat_GF2& data_matrix,mat_GF2& encoding_matrix)
{
    mat_GF2 out_matrix;
    out_matrix=encoding_matrix*data_matrix;
    return out_matrix;
}

mat_GF2 decoded_data(mat_GF2& encoded_data_matrix,mat_GF2& inverse_matrix)
{
    mat_GF2 out_matrix;
    out_matrix=inverse_matrix*encoded_data_matrix;
    return out_matrix;
}

void binary_to_char(char* output_data, mat_GF2& X)
{
    //char* data=(char*)malloc(sizeof(X[0][0])*X.NumCols()*X.NumRows());
    bitset<8> bits;
    bits.reset();
    int s=0;
    int q=0;
    for (int i=0; i<X.NumRows(); i++)
    {
        for (int j=0; j<X.NumCols(); j++)
        {
            if (X[i][j]==0)
                bits[s]=0;
            else
                bits[s]=1;
            s++;
            if (s%8==0)
            {
                //cout<<bits<<endl;
                output_data[q]=static_cast<char>(bits.to_ulong());
                bits.reset();
                s=0;
                q++;

            }
        }
    }
    if (s!=0)
        output_data[q]=static_cast<char>(bits.to_ulong());
}

unsigned int binary_to_unsigned_int(mat_GF2& X)
{
    bitset<32> bits;
    bits.reset();
    int s=0;
    int q=0;
    unsigned int out;
    for (int i=0; i<X.NumRows(); i++)
    {
        for (int j=0; j<X.NumCols(); j++)
        {
            if (X[i][j]==0)
                bits[s]=0;
            else
                bits[s]=1;
            s++;
            if (s%32==0)
            {
                //cout<<bits<<endl;
                out=static_cast<unsigned int>(bits.to_ulong());
                bits.reset();
                s=0;
                q++;

            }
        }
    }
    return out;
}


void XOR_encode(mat_GF2& X, vector<char*>& data, char* out_payload)
{
    char* first=data.at(0);
    int payload_length=sizeof(*first)/sizeof(first[0]); 
    payload_length=PAYLOAD_SIZE;
    char sum;
    // TODO invert the 2 cycles
    //out_payload=(char *)calloc(payload_length,sizeof(char*));
    for (int j=0; j<payload_length; j++)
    {
        sum=0;
        for (int i=0; i<X.NumCols(); i++)
        {
            if (X[0][i]==1)
            {
                char* pyl=data.at(i);
                sum=sum ^ pyl[j];
            }
        }
        
        out_payload[j]=sum;
    }
}

vector<char*> XOR_decode(mat_GF2& X, vector<char*>& encoded_data)
{
    char* first=encoded_data.at(0);
    int payload_length=sizeof(first)/sizeof(first[0]);
    payload_length=PAYLOAD_SIZE;
    vector<char*> out;
    char* tmp_decoded=(char *)calloc(payload_length, sizeof(char));
    // TODO invert h and j cycles
    for (int i=0; i<X.NumRows(); i++)
    {
        out.push_back((char *)calloc(payload_length, sizeof(char)));
        for (int h=0; h<payload_length; h++) 
        {
            char sum=0;
            for (int j=0; j<X.NumCols(); j++)
            {
                if (X[i][j]==1)
                {
                    char* pyl=encoded_data.at(j);
                    sum=sum ^ pyl[h];
                }
                tmp_decoded[h]=sum;
            }
        }
        memcpy(out.at(i), tmp_decoded, payload_length);
    }
    return out;
}


