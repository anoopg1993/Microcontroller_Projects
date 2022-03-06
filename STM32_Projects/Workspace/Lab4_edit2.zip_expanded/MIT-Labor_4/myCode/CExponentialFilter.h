/*
 * CExponentialFilter.h
 *
 *  Created on: 29-Nov-2021
 *      Author: vrind
 */

#ifndef CEXPONENTIALFILTER_H_
#define CEXPONENTIALFILTER_H_

/**
 * Modelliert die Realisierung von exponentieller Glättung
 */

class CExponentialFilter {
public:
	CExponentialFilter(float factor);
	CExponentialFilter& operator<<(float value);
	operator float() const;

private:
	float m_factor;
	float m_last;
};

#endif /* CEXPONENTIALFILTER_H_ */
