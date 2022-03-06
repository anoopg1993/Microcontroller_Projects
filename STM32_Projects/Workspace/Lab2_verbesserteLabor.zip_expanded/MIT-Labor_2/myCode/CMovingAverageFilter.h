/*
 * CMovingAverageFilter.h
 *
 *  Created on: 28-Nov-2021
 *      Author: vrind
 */

#ifndef CMOVINGAVERAGEFILTER_H_
#define CMOVINGAVERAGEFILTER_H_

#include"stdint.h"

/**
 * Modelliert die Bildung eines gleitenden Mittelwerts von einer Datenreihe.
 */

class CMovingAverageFilter {
public:
	CMovingAverageFilter(uint16_t size = 16);
	CMovingAverageFilter& operator<<(uint16_t value);
	//uint16_t operator uint16_t () const;                 //In C++ the operator overloading cannnot be done this way
	operator uint16_t () const;
private:
	uint16_t* m_values;
	uint16_t m_valuesSize;
	uint16_t m_nextIndex =0;
	uint32_t m_sum ;


};

#endif /* CMOVINGAVERAGEFILTER_H_ */
