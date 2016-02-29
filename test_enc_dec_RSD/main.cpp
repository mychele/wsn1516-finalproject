#include <iostream>
#include"utils_wsn.h"
#include <algorithm>

using namespace std;



int main()
{
    int K=2700;
    int N=3000;
    int right_node;

    double c=0.03;
    double delta=0.5;
    srand(time(NULL));
    mat_GF2 M=rand_create_sparse_matrix(N,K,rand(),c,delta);
    cout<<"ecoding matrix"<<endl;
    //write_matrix(M,0);
    //vector of incoming nodes for each right node, representing and encoded packet (look at figure on Rossi slides on LT)
    std::vector<std::vector<unsigned short int> > u(N);
    std::vector<std::vector<unsigned short int> > e(N);
    //vector of outcoming nodes for each left node, representing and unecoded data pck
    std::vector<std::vector<unsigned short int> > v(K);
    std::vector<std::vector<unsigned short int> > d(K);
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<K; j++)
        {
            if (M[i][j]==1)
            {
                u[i].push_back(j);
                v[j].push_back(i);
            }

        }

    }
    for (int j=0; j<K; j++)
    {
        if (v[j].size()==0)
        {
            cout<<"there's an all 0 column! Impossible to decode"<<endl;
            return -2;
        }

    }
    cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;
    std::cout<<"inital situation: vector u:"<<endl;
    for (int i=0; i<N; i++)
    {
        for (std::vector<unsigned short int>::iterator it = u[i].begin(); it != u[i].end(); ++it)
            std::cout << *it << ' ';
        cout<<endl<<"--------"<<endl;
    }
    cout<<"--------------------------"<<endl;
    std::cout<<"inital situation: vector v:"<<endl;
    for (int i=0; i<K; i++)
    {
        for (std::vector<unsigned short int>::iterator it = v[i].begin(); it != v[i].end(); ++it)
            std::cout << *it << ' ';
        cout<<endl<<"--------"<<endl;
    }
    cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;
    cout<<"end of preparations"<<endl;
    bool something_done=0;
    unsigned short int pivot;
    //edge: left node (pivot), right node
    std::vector<unsigned short int> pivot_vector;
    int current_node;
    do
    {
        something_done=0;
        for (int i=0; i<N; i++)
        {
            //one edge: remove it and note how left node is decoded (vectors d and e)
            if (u[i].size()==1)
            {
                something_done=1;
                pivot=u[i].back();
                //cout<<"u["<<i<<"].back is "<<u[i].back()<<endl;
                pivot_vector.push_back(pivot);
                //aggiungi il nodo stesso
                e[i].push_back(i);
                u[i].pop_back();
                //altrimenti il nodo di sinistra è già stato decodificato in precedenza, e non c'è bisogno di farlo nuovamente
                if (d[pivot].size()==0)
                {
                    for (int k=0; k<e[i].size(); k++)
                        d[pivot].push_back(e[i].at(k));
                }
                //la combinazione di XOR a e[i] viene cancellata e poi riscritta al passaggio successivo (eliminazione degli archi da sinistra a destra).
                //questo non è un problema poiché la combinazione di XOR a sinistra non viene mai aggiornata (viene cioè creata una volta in corrispondenza
                //di una eliminazione da destra a sinistra, e poi rimane sempre uguale
                //il motivo è che così non si deve memorizzare il nodo a destra i al quale punta il nodo pivot a sinistra (altrimenti quando si aggiornano gli
                //XOR a destra verrebbe i XOR i
                e[i].clear();
            }
        }
        while (pivot_vector.size()>0 && something_done)
        {
            pivot=pivot_vector.back();
            //cout<<"pivot is "<<pivot<<endl;
            pivot_vector.pop_back();
            for (int j=v[pivot].size()-1; j>=0; j--)
            {
                //current right node
                current_node=v[pivot].back();
                //aggiorna gli XOR a destra (aggiungi la codifica del nodo pivot)
                for (int k=0; k<d[pivot].size(); k++)
                {
                    e[current_node].push_back(d[pivot].at(k));

                }
                //elimina l'arco (due archi orientati) che collega v[pivot] a v[pivot].at(j)
                v[pivot].pop_back();
                for (int k=0; k<u[current_node].size(); k++)
                {
                    if (u[current_node].at(k)==pivot)
                    {
                        u[current_node].erase(u[current_node].begin()+k);
                        break;
                    }

                }

            }
        }

        /*cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;
        std::cout<<"iteration "<<q<<":  vector u:"<<endl;
        for (int i=0; i<N; i++)
        {
            for (std::vector<int>::iterator it = u[i].begin(); it != u[i].end(); ++it)
                std::cout << *it << ' ';
            cout<<endl<<"--------"<<endl;
        }
        cout<<"--------------------------"<<endl;
        std::cout<<"iteration "<<q<<":  vector v:"<<endl;
        for (int i=0; i<K; i++)
        {
            for (std::vector<int>::iterator it = v[i].begin(); it != v[i].end(); ++it)
                std::cout << *it << ' ';
            cout<<endl<<"--------"<<endl;
        }
        std::cout<<"iteration "<<q<<":  vector e:"<<endl;
        for (int i=0; i<N; i++)
        {
            for (std::vector<int>::iterator it = e[i].begin(); it != e[i].end(); ++it)
                std::cout << *it << ' ';
            cout<<endl<<"--------"<<endl;
        }
        std::cout<<"iteration "<<q<<":  vector d:"<<endl;
        for (int i=0; i<K; i++)
        {
            for (std::vector<int>::iterator it = d[i].begin(); it != d[i].end(); ++it)
                std::cout << *it << ' ';
            cout<<endl<<"--------"<<endl;
        }
        cout<<"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"<<endl;*/
    }
    while (something_done);



//print final vectors
//    cout<<"vector u:"<<endl;
//    for (int i=0; i<N; i++)
//    {
//        for (std::vector<int>::iterator it = u[i].begin(); it != u[i].end(); ++it)
//            std::cout << *it << ' ';
//        cout<<endl<<"--------"<<endl;
//    }
//    cout<<"vector v:"<<endl;
//    for (int i=0; i<K; i++)
//    {
//        for (std::vector<int>::iterator it = v[i].begin(); it != v[i].end(); ++it)
//            std::cout << *it << ' ';
//        cout<<endl<<"--------"<<endl;
//    }
//    cout<<"vector e:"<<endl;
//    for (int i=0; i<N; i++)
//    {
//        for (std::vector<int>::iterator it = e[i].begin(); it != e[i].end(); ++it)
//            std::cout << *it << ' ';
//        cout<<endl<<"--------"<<endl;
//    }
//    cout<<"vector d:"<<endl;
//    for (int i=0; i<K; i++)
//    {
//        for (std::vector<int>::iterator it = d[i].begin(); it != d[i].end(); ++it)
//            std::cout << *it << ' ';
//        cout<<endl<<"--------"<<endl;
//    }
    for (int i=0; i<K; i++)
    {
        if (d[i].size()==0)
        {
            cout<<"decoding failed!"<<endl;
            return -2;
        }

    }
    cout<<"decoding successful!"<<endl;

    const int m=40;
    mat_GF2 data=rand_create_data(K, m,rand());
    cout<<"data \n";
    //write_matrix(data,0);
    cout<<"\n";
    mat_GF2 data_enc=encoded_data(data,M);
    cout<<"encoded data \n";
    //write_matrix(data_enc,0);
    cout<<"\n";
    mat_GF2 M_inv;
    M_inv.SetDims(K,N);
    for (int i=0; i<K; i++)
    {
        for (int j=0; j<N; j++)
        {
            M_inv[i][j]=0;

        }
    }
    for (int i=0; i<d.size(); i++)
    {
        for (int j=0; j<d[i].size(); j++)
        {
            M_inv[i][d[i].at(j)]=M_inv[i][d[i].at(j)]+1;

        }
    }
    cout<<"decoding matrix \n";
    //write_matrix(M_inv,0);
    cout<<"\n";
    mat_GF2 data_dec=decoded_data(data_enc,M_inv);
    cout<<"decoded data \n";
    //write_matrix(data_dec,0);
    cout<<"\n";
    cout<<"data and decoded data are equal?\n";
    long equality_check=(data==data_dec);
    cout<<equality_check<<"\n";


    return 0;
}
