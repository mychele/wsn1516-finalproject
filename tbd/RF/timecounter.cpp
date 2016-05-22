/**
 * This is the source file for TimeCounter class
 */

#include <iostream>
#include <chrono>
#include <stdlib.h>
#include "timecounter.h"

TimeCounter::TimeCounter(microseconds defaultValue) 
{
	m_sEst = defaultValue;
	m_estVar = defaultValue/2;
	m_beta = 1;
	m_beta_den = 4;
	m_alpha = 1;
	m_alpha_den = 8;
	m_k = 4;
	m_minimumValue = 1;
}

// see RFC 6298
void 
TimeCounter::update(microseconds value)
{
	// compute the difference in this way, avoids using a std::abs
	microseconds s_diff = (value - m_sEst < microseconds(0)) ? (m_sEst - value) : (value - m_sEst);
	// update variance
	m_estVar = ((m_beta_den - m_beta)*m_estVar + m_beta*s_diff)/m_beta_den;
	// update estimate
	m_sEst = ((m_alpha_den - m_alpha)*m_sEst + m_alpha*value)/m_alpha_den;
}


microseconds
TimeCounter::get() const
{
	// return the minimum value or the estimate
	microseconds toBeReturned = (m_sEst + m_k*m_estVar < std::chrono::milliseconds(m_minimumValue)) ? std::chrono::milliseconds(m_minimumValue) : (m_sEst + m_k*m_estVar);
	return toBeReturned;
}


