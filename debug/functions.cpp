#include "functions.h"
#include <bitset>

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
                cout<<bits<<endl;
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
                cout<<bits<<endl;
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
    cout<<"ok3.21\n";
    char* first=data.at(0);
    int payload_length=sizeof(first)/sizeof(first[0]);
    out_payload=(char *)malloc(sizeof(char)*payload_length);
    char sum;
    for (int j=0; j<payload_length; j++)
    {
        cout<<"ok3.22\n";
        sum=0;
        for (int i=0; i<X.NumCols(); i++)
        {
            if (X[0][i]==1)
            {
                cout<<"ok3.23\n";
                char* pyl=data.at(i);
                cout<<"ok3.24\n";
                sum=sum ^ pyl[j];
                cout<<"ok3.25\n";
            }
        }

        cout<<"ok3.26\n";
        out_payload[j]=sum;
    }
    cout<<"ok3.28\n";
}

vector<char*> XOR_decode(mat_GF2& X, vector<char*>& encoded_data)
{
    cout<<"ok5.51\n";
    char* first=encoded_data.at(0);
    int payload_length=sizeof(first)/sizeof(first[0]);
    vector<char*> out;
    cout<<"ok5.52\n";
    char* tmp_decoded=(char *)malloc(sizeof(char)*payload_length);
    cout<<"numrows of decoding matrix: "<<X.NumRows()<<endl;
    for (int i=0; i<X.NumRows(); i++)
    {
        for (int h=0; h<payload_length; h++)
        {
            cout<<"ok5.53\n";
            char sum=0;
            for (int j=0; j<X.NumCols(); j++)
            {
                if (X[i][j]==1)
                {
                    char* pyl=encoded_data.at(i);
                    sum=sum ^ pyl[h];
                }
                cout<<"ok5.54\n";
                tmp_decoded[j]=sum;
                cout<<"ok5.55\n";
            }
        }
        cout<<"ok5.56\n";
        out.push_back(tmp_decoded);
        cout<<"ok5.57\n";
    }
    cout<<"ok5.58\n";
    return out;
}

