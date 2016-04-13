/**
 * This is the header file for TimeCounter class
 */

#include <chrono>

#ifndef TIMECOUNTER
#define TIMECOUNTER

using std::chrono::microseconds;

/**
 * A TimeCounter object can be used to estimate the time which is expected between two 
 * events. Its method get returns an estimate of the time interval, that can be used
 * for example to set a timeout. Its method update is used to feed new measures whenever
 * they are available, so that the estimate becomes more and more reliable
 */
class TimeCounter {

	microseconds m_sEst;
	microseconds m_estVar;
	int m_beta;
	int m_beta_den;
	int m_alpha;
	int m_alpha_den;
	int m_k;

public:

	TimeCounter();

    /**
	 * Public constructor
	 * @param an initial value for the timeout, as a std::chrono::microseconds
	 */
    TimeCounter(microseconds defaultValue);

	/**
	 * Update the estimate when a new measure is available
	 * @param a std::chrono::microseconds with a new measure
	 */
	void update(microseconds value);

	/**
	 * Get the current estimate
	 * @return the current estimate in microseconds
	 */
	microseconds get() const;

};

#endif
//TIMECOUNTER
