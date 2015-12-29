#include "Packet.h"

Packet::Packet(mat_GF2 X)
{
    int const K=X.NumRows();
    ev=rand_create_matrix(1,K);
    encoded_data=ev*X;
}
mat_GF2 Packet::get_EV(void)
{
    return ev;
}

mat_GF2 Packet::get_Encoded(void)
{

    return encoded_data;
}

mat_GF2 Packet::decode(mat_GF2 X)
{
    mat_GF2 decoded_data;
    decoded_data=X*encoded_data;
    return decoded_data;
}
