/*
 * CMovingAverageFilter.cpp
 *
 *  Created on: 28-Nov-2021
 *      Author: vrind
 */

#include "CMovingAverageFilter.h"

/*Creates a new filter
 * @param size Arraysize which is the number of values to be used from measured data.
 */
CMovingAverageFilter::CMovingAverageFilter(uint16_t size) {
	    m_values = new uint16_t[size];
		for (int i = 0; i < size; i++){
			m_values[i] = 0;                                         //The Array is with 0 initialized
		}
		m_valuesSize = size;
		m_sum = 0;                                                   //Variable to store the sum of array
		m_nextIndex = 0;
}

/**
 * Applies a new value that replaces the oldest entry.
 * @param value - the new measured data
 */
CMovingAverageFilter& CMovingAverageFilter::operator<<(uint16_t value){
	    m_sum -= m_values[m_nextIndex];                               //Oldest measured data is subtracted
		m_values[m_nextIndex] = value;
		m_sum += value;                                               //The new measured data is added to the sum
		m_nextIndex = (m_nextIndex+1)%m_valuesSize;                   //Increment the m_nextIndex by 1 and modulo it after the end Index so  starts again from 0
		return *this;

}

/**
 * Gives the filtered value i.e. the average
 */
CMovingAverageFilter::operator uint16_t() const{
	return m_sum / m_valuesSize;                                      //Average = Sum / Number of measured data N
}
