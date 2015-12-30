//http://www.shoup.net/ntl/

#include <iostream>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<NTL/ZZ.h>
#include<NTL/vector.h>
#include<NTL/GF2X.h>
#include<NTL/GF2.h>
#include<NTL/mat_GF2.h>"
#include "decoding_function.h"
#include "NCpacket.h"
#include "utils_wsn.h"
#include "Packet.h"
#include "functions.h"


using namespace std;
using namespace NTL;


int main()
{
    srand(time(0));
    int const K=10;
    bool decoded=0;
    int N_init=6;
    int N=N_init; //nb of rows of data encoded matrix=nb of rows of encoding matrix=nb of sent packets=number of EVs
    int N_more=3; //nb of additional pcks to send if encoding matrix is not full rank (K)
    const int m=15; //nb of bits per data packet
    mat_GF2 data=rand_create_data(K, m);
    cout<<"data \n";
    write_matrix(data,0);
    cout<<"\n";
    mat_GF2 data_enc;
    data_enc.SetDims(N,m);
    mat_GF2 M;
    M.SetDims(N,K);

    for (int i=0; i<N; i++)
    {
        mat_GF2 tmp_ev, tmp_encoded;
        Packet pck=Packet(data);
        tmp_ev=pck.get_EV();
        tmp_encoded=pck.get_Encoded();
        for (int s=0; s<K; s++)
        {
            M[i][s]=tmp_ev[0][s];
        }
        for (int s=0; s<m; s++)
        {
            data_enc[i][s]=tmp_encoded[0][s];

        }
    }

    do
    {
        cout<<"encoded data \n";
        write_matrix(data_enc,0);
        cout<<"\n";
        mat_GF2 M_id;
        M_id=append_identity(M);
        cout<<"matrix M (encoding matrix) is \n";
        write_matrix(M,0);
        cout<<"\n";
        cout<<"matrix M_id is \n";
        write_matrix(M_id,1);
        cout<<"\n";
        gauss(M_id);
        cout<<"gaussian elimination of M_id\n";
        write_matrix(M_id,1);
        cout<<"\n";
        //index of last nonzero row
        int last_nonzero=N-1;
        int flag_nonzero=0;
        do
        {
            flag_nonzero=0;
            for (int j=0; j<K; j++)
            {
                if (M_id[last_nonzero][j]!=0)
                    flag_nonzero=1;
            }
            if (flag_nonzero==0)
                last_nonzero--;
            else break;
        }
        while (last_nonzero>=0);
        cout<<"last nonzero row index (starting from 0)= "<<last_nonzero<<"\n";
        cout<<"K-1= "<<K-1<<"\n";
        if (last_nonzero<K-1)
        {
            cout<<"=====================more rows needed!! send more pls!! =============================\n";
            N=N+N_more;
            cout<<"N= "<<N<<"\n";
            M.SetDims(N,K);
            data_enc.SetDims(N,m);
            for (int i=N-N_more; i<N; i++)
            {
                mat_GF2 tmp_ev, tmp_encoded;
                Packet pck=Packet(data);
                tmp_ev=pck.get_EV();
                tmp_encoded=pck.get_Encoded();
                for (int s=0; s<K; s++)
                {
                    M[i][s]=tmp_ev[0][s];
                }
                for (int s=0; s<m; s++)
                {
                    data_enc[i][s]=tmp_encoded[0][s];

                }
            }
        }
        else
        {
            decoded=1;
            for (int j=K-1; j>=0; j--)
            {
                for (int i=j-1; i>=0; i--)
                {
                    if (M_id[i][j]==1)
                    {
                        for (int s=0; s<M_id.NumCols(); s++)
                        {
                            M_id[i][s]=M_id[i][s]+M_id[j][s];
                        }

                    }

                }
            }

            cout<<"gaussian-jordan elimination\n";
            write_matrix(M_id,1);
            cout<<"\n";
            mat_GF2 M_inv=pseudo_inverse(M_id, last_nonzero+1);
            cout<<"inverse\n";
            write_matrix(M_inv,0);
            cout<<"\n";
            mat_GF2 data_dec=decoded_data(data_enc,M_inv);
            cout<<"decoded data \n";
            write_matrix(data_dec,0);
            cout<<"\n";
            cout<<"data and decoded data are equal?\n";
            long equality_check=(data==data_dec);
            cout<<equality_check<<"\n";
        }
    }
    while(!decoded);
    int const R=sizeof(data[0][0])*data.NumCols()*data.NumRows();
    char payload[R];
    char *p=payload;
    binary_to_char(p, data);
    cout<<payload[0]<<endl;
    cout<<payload[0]+'0'<<endl;
    cout<<"integer"<<endl;
    mat_GF2 prova;
    prova.SetDims(1,32);
    for (int i=0; i<12; i++)
        prova[0][i]=rand()%2;
    cout<<prova<<endl;
    unsigned int out=binary_to_unsigned_int(prova);
    cout<<out<<endl;
    return 1;
}
