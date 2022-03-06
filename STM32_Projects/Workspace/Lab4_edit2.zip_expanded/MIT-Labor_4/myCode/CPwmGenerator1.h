/*
 * CPwmTest1.h
 *
 *  Created on: 19.12.2020
 *      Author: mnl
 */

#ifndef CPWMGENERATOR1_H_
#define CPWMGENERATOR1_H_

/**
 * Modelliert unter Nutzung des Timer 3 einen Generator für ein
 * PWM-Signal auf PC6. Die Periode ist mit einer Ausgangsfrequenz des
 * Prescalers von ca. 2048 Hz und einem Counter-Modulo von 16*2048 fest
 * auf 16 s eingestellt, das Ein-/ausverhältnis auf 10:6.
 */
class CPwmGenerator1 {
private:
	/**
	 * Konfiguriert PC6 als Alternate Function-Ausgang und selektiert
	 * den von Timer 3/Compare Register 1 generierten Wert als das
	 * auszugebende Signal.
	 */
	void configureGpioPin();

	/**
	 * Konfiguriert den Timer 3 inklusive des Compare Register 1 so,
	 * dass ein Signal wie in der Klassenbeschreibung dargestellt
	 * erzeugt wird.
	 */
	void configureTimer();

public:
	CPwmGenerator1();
};

#endif /* CPWMGENERATOR1_H_ */
