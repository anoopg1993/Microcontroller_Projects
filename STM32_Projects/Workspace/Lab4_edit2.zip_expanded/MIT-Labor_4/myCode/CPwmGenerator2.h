/*
 * CPwmGenerator2.h
 *
 *  Created on: 22-Jan-2022
 *      Author: vrinda
 */

#ifndef CPWMGENERATOR2_H_
#define CPWMGENERATOR2_H_

/**
 * A model which generates using the Timer 3 a PWM signal on PC6.
 * The period is set with an output frequency of the Prescalers of approx. 2048 Hz
 * and a counter-modulo of 16*2048 set to 16 seconds,
 * the on/off ratio is controlled through the potentiometer.
 * Potentiometer -> 0% (Potentiometer turned complete left).
 * Potentiometer -> 100% (Potentiometer turned complete right).
 */
#include <stdint.h>
class CPwmGenerator2 {
private:
	/**
	 * Configure PC6 as an Alternate Function-Output and the generated value from
	 * the Timer 3/Compare Register 1 as the output Signal
	 */
	void configureGpioPin();

	/**
	 * Configure the Timer 3 including the Compare Register 1 so
	 * that a Signal described in the Class is created
	 */
	void configureTimer();
public:
	/**
	 * Constructor
	 */
	CPwmGenerator2();

	/**
	 * Set the duty cycle of PWM-Signal
	 * A duty cycle  is the fraction of one period in which a signal or system is active.
	 * The percentage > 100 are treated as 100.
	 *
	 * @param percent the percentage where the system remains active
	 */
	void setDutyCycle (uint8_t percent);
};

#endif /* CPWMGENERATOR2_H_ */
