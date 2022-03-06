/*
 * CExponentialFilter.cpp
 *
 *  Created on: 29-Nov-2021
 *      Author: vrind
 */

#include "CExponentialFilter.h"

/*
 * Creates a new exponential filter
 * @param factor - the smoothing factor
 * The greater the smoothing factor, the more influence the new value
 * has on the smoothened value
 */
CExponentialFilter::CExponentialFilter(float factor) {
	m_factor = factor;
	m_last = 0;
}

/*
 *The new value which is filtered with the last value
 *@param value - the new value
 */
CExponentialFilter& CExponentialFilter::operator<<(float value){
	m_last = m_last + (m_factor*(value-m_last));
	return *this;
}
/*
 * Returns the filtered value
 */
CExponentialFilter::operator float() const {
	return m_last;
}
