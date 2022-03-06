/*
 * CPwmGenerator3.h
 *
 *  Created on: 22-Jan-2022
 *      Author: vrinda
 */

#ifndef CPWMGENERATOR3_H_
#define CPWMGENERATOR3_H_

#include <stdint.h>
/**
 * A model which generates using the Timer 3 a PWM signal on PC6.
 * The period can be set between 1µs to 65536µs which is given as a parameter in the constructor
 * The on/off ratio i.e. duty cycle is controlled through the potentiometer.
 * Potentiometer -> 0% (Potentiometer turned complete left).
 * Potentiometer -> 100% (Potentiometer turned complete right).
 */
class CPwmGenerator3 {
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

	/**
	 * Attribut, in which the time period is saved
	 * Through the time period duration the time for LED to light(to remain active) is controlled
	 */
	uint16_t m_period;
public:
	/**
	 * Constructor
	 *
	 * @param period it is the time period given in microseconds of PWM-Signal
	 */
	CPwmGenerator3(uint16_t period);

	/**
	 * Set the duty cycle of PWM-Signal
	 * A duty cycle  is the fraction of one period in which a signal or system is active.
	 * The percentage > 100 are treated as 100.
	 *
	 * @param percent the percentage where the system remains active
	 */
	void setDutyCycle (uint8_t percent);
};

#endif /* CPWMGENERATOR3_H_ */
