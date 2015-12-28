#include "functions.h"

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

    //or create Id matrix and just append:
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
