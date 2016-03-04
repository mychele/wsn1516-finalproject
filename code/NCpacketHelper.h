/**
 * It initializes and keeps in memory the
 * Robust Soliton Distribution, so that is computed just once
 */

#include <random>
#include "NCpacket.h"

#ifndef NCPACKETHELPER
#define NCPACKETHELPER

class NCpacketHelper {

public:

	NCpacketHelper();

	/**
	 * Constructor that sets the RSD parameters
	 * @param K the maximum degree
	 * @param c_rsd the param c in RSD
	 * @param delta_rsd the param delta in RSD
	 */
	NCpacketHelper(int K, double c_rsd, double delta_rsd);

	/**
	 * Set the RSD parameters
	 * @param K the maximum degree
	 * @param c_rsd the param c in RSD
	 * @param delta_rsd the param delta in RSD
	 */
    void setRSDparam(int K, double c_rsd, double delta_rsd);

    /**
     * Get the binary header as position of 1 in the encoding vector
     * @param the seed for the rng
     */ 
    std::vector<int> getBinaryHeader(int seed);

    /**
     * Create an NCpacket
     * @param the pointer to the data vector
     * @param the blockID
     */
	NCpacket createNCpacket(std::vector<char*> *data, unsigned char blockID);

private:
	// RSD param and pmd and cdf vectors
	double m_delta_rsd;
	double m_c_rsd;
	int m_K;
	std::vector<double> m_pmd;
	std::vector<double> m_cdf;
	// rng objects
	std::random_device m_randomDevice; // device entropy
    std::mt19937 m_eng;
    std::uniform_real_distribution<double> m_distr01; // it will create uniform random number in range 0,1 
    std::uniform_int_distribution<int> m_distr0k; // it will create uniform random number in range 0,1 

    void initRSDpmd();
    void initRSDcdf();

};

#endif
//NCPACKETHELPER
