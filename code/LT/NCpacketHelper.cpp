#include "NCpacketHelper.h"
#include <iostream>

NCpacketHelper::NCpacketHelper(int K, double c_rsd, double delta_rsd) {
	m_delta_rsd = delta_rsd;
	m_c_rsd = c_rsd;
	m_K = K;
	m_pmd = std::vector<double>(m_K, 0.0);
	m_cdf = std::vector<double>(m_K, 0.0);
	
	// initialize the pmd and cdf
	initRSDpmd();
	initRSDcdf();

	// initialize the random engine and the distribution
	//m_randomDevice; // device entropy
	m_eng = std::mt19937(m_randomDevice());
    m_distr01 = std::uniform_real_distribution<double>(0.0, 1.0); // it will create uniform random number in range 0,1 
    m_distr0k = std::uniform_int_distribution<int>(0, m_K - 1);
}

void
NCpacketHelper::setRSDparam(int K, double c_rsd, double delta_rsd) {
	m_delta_rsd = delta_rsd;
	m_c_rsd = c_rsd;
	m_K = K;

	// initialize the pmd and cdf
	initRSDpmd();
	initRSDcdf();
}

void
NCpacketHelper::initRSDpmd() {

    double S = m_c_rsd * std::log( (double)m_K/m_delta_rsd ) * std::sqrt( (double)m_K );
    int K_S = (int)(m_K/S);
    K_S = std::min(K_S,m_K);

    std::vector<double> rho(m_K);
    std::vector<double> tau(m_K,0.0);
    rho[0] = 1/m_K;
    for(int i = 1; i < m_K; i++) {
        rho[i] = (double)1/((i*(i+1)));
    }

    for (int i = 0; i < K_S-1; i++) {
        tau[i] = (double)S/((i+1)*m_K);
    }
    tau[K_S-1] = S/m_K*std::log(S/m_delta_rsd);
	
	//compute normalizations constants and normalize
    double normalization = 0;
    for (int i = 0; i < m_K; i++) {
        normalization = (double)normalization+rho[i]+tau[i];
    }
    for (int i = 0; i < m_K; i++) {
        m_pmd[i] = (double)(rho[i]+tau[i])/normalization;
    }
}

void 
NCpacketHelper::initRSDcdf() {
    for (int i = 1; i < m_K; i++)
    {
        m_cdf[i] = m_cdf[i - 1] + m_pmd[i - 1];
    }
}

std::vector<int> 
NCpacketHelper::getBinaryHeader(int seed) {
	// set the seed
	m_eng.seed(seed);
    double rnd_num = m_distr01(m_eng); // U(0,1) number
    int row_degree = 0;
    // CDF inversion
    for(int i = 0; i < m_K; ++i) {
		if(m_cdf[i] > rnd_num) {
			row_degree = i;
			break;
		}
    }
    // create the vector of 1's positions
    std::vector<int> onePositions = std::vector<int>(row_degree);
    for(int i = 0; i < row_degree; i++) {
    	// get a random number in [0, K-1] and save it in the vector
		onePositions[i] = m_distr0k(m_eng);
    }
    return onePositions;
}

NCpacket
NCpacketHelper::createNCpacket(std::vector<char*> *data, unsigned char blockID) {
	m_eng.seed(m_randomDevice());
	int header = m_eng(); // create a random header using device entropy
	return NCpacket(header, blockID, getBinaryHeader(header), data);
}



