//http://www.shoup.net/ntl/

#include <iostream>
#include<stdio.h>
#include<math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <NTL/ZZ.h>
#include <NTL/vector.h>
#include <NTL/GF2X.h>
#include <NTL/mat_GF2.h>



using namespace std;
using namespace NTL;


void rand_initialize_matrix(mat_GF2& X, int const r, int const c);
mat_GF2 rand_create_matrix(int const r, int const c);

//K=number of packets, m=bits per packet
mat_GF2 rand_create_data(int const K, int const m);
mat_GF2 append_identity(mat_GF2& X);
mat_GF2 encoded_data(mat_GF2& data_matrix,mat_GF2& encoding_matrix);
mat_GF2 decoded_data(mat_GF2& _encoded_data_matrix,mat_GF2& inverse_matrix);
void write_matrix(mat_GF2& X, bool id_appended);

mat_GF2 pseudo_inverse(mat_GF2& X, int const matrix_rank);

int main()
{
    srand(time(0));
    const int r=20, c=10;
    const int c_incr=c+r;
    mat_GF2 M;

    /*int A[r][c]={1,1,1,  1,1,1,1,1,1,1,1,1};
    mat_GF2 matr1;
    matr1.SetDims(r,c_incr);
    for (int i=0;i<r;i++){
        for (int j=0;j<c;j++){
            matr1[i][j]=A[i][j];
        }
    }*/

    M=rand_create_matrix(r,c);
    mat_GF2 M_id;
    M_id=append_identity(M);
    cout<<"matrix M is \n";
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
    int last_nonzero=r-1;
    int flag_nonzero=0;
    do
    {
        flag_nonzero=0;
        for (int j=0; j<c; j++)
        {
            if (M_id[last_nonzero][j]!=0)
                flag_nonzero=1;
        }
        if (flag_nonzero==0)
            last_nonzero--;
        else break;
    }
    while (last_nonzero>=0);
    cout<<"last nonzero row index (starting from 0) \n"<<last_nonzero<<"\n";
    if (last_nonzero<c-1)
    {
        cout<<"more rows needed!! send more pls!! \n";
        return 1;
    }
    else
        for (int j=c-1; j>=0; j--)
        {
            for (int i=j-1; i>=0; i--)
            {
                if (M_id[i][j]==1)
                {
                    for (int s=0; s<c_incr; s++)
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
    mat_GF2 data=rand_create_data(c, 15);
    cout<<"data \n";
    write_matrix(data,0);
    cout<<"\n";
    mat_GF2 data_enc=encoded_data(data,M);
    cout<<"encoded data \n";
    write_matrix(data_enc,0);
    cout<<"\n";
    mat_GF2 data_dec=decoded_data(data_enc,M_inv);
    cout<<"decoded data \n";
    write_matrix(data_dec,0);
    cout<<"\n";
    cout<<"data and decoded data are equal?\n";
    long equality_check=(data==data_dec);
    cout<<equality_check<<"\n";
    return 1;
}


void rand_initialize_matrix(mat_GF2& X, int const r, int const c)
{
    int n;
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
    /*
    //append identity matrix to the right
    for (int i=0;i<rows;i++){
        for (int j=columns;j<columns_incr;j++){
            if (j-columns==i)
                out_matrix[i][j]=1;
            else
                out_matrix[i][j]=0;
        }
    }*/

    //or to append:
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
