/*
 * CSevenSegmentDisplay.h
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#ifndef CSEVENSEGMENTDISPLAY_H_
#define CSEVENSEGMENTDISPLAY_H_

#include "CShiftRegisterOutputExtender.h"

extern "C" void TIM1_UP_TIM10_IRQHandler();
/**
 * Modelliert eine 4-stellige 7-Segment-Anzeige. Um eine "aktive" und
 * flackerfreie Darstellung zu erhalten, muss sichergestellt werden,
 * dass die Methode "activateNextDigit" mindestens alle 5 ms aufgerufen wird.
 */
class CSevenSegmentDisplay {
	/**
	 * Declaration of the Interrupt Handlers of Timer 10 as
	 * friend function, so that the private Methode like activateNextDigt
	 * und shutDownAllDigits from the main.cpp can be called
	 */
	friend void TIM1_UP_TIM10_IRQHandler();
private:
	static uint8_t patterns[];
	CShiftRegisterOutputExtender* m_ssdSegments;
	DigitalOut* m_digits[4];
	uint8_t m_activeDigit;
	uint8_t m_values[4];

	//bool m_setDecimalPoint[4] = {false};
	int m_pointValue;

	static CSevenSegmentDisplay* m_instance;

	float m_brightness;
	/**
	 * Help Method for the Time Division Multiplex. Activates the next Display position
	 */
	void activateNextDigit();

	/**
	 * Configure Timer 10 which includes Prescaler, Auto-Reload und Compare Register
	 * Enable the Interrupts and trigger a Update-Event.
	 * Then a timer is activated
	 */
	void initTimer();
	/**
	 * turn off all the digits to 0
	 */
	void shutDownAllDigits();
public:
	/**
	 * Erzeugt ein neues Objekt, das eine 4-stellige 7-Segment-Anzeige
	 * ansteuert, deren Segmente (und Dezimalpunkt) an das über
	 * ssdSegments ansteuerbare Schieberegister angeschlossen sind und
	 * deren Anzeigestellen über die übergebenen digitalen Ausgänge
	 * aktiviert werden können.
	 *
	 * @param ssdSegments die Modellierung des Schieberegisters zur
	 *  Ansteuerung der Segemente
	 * @param digits die Ausgänge zum Aktivieren der Anzeigestellen
	 */
	CSevenSegmentDisplay(CShiftRegisterOutputExtender* ssdSegments,
			DigitalOut* digits[4]);



	/**
	 * Ermöglicht den Zugriff auf die an der ausgewählten Anzeigestelle
	 * darzustellende Ziffer. Da eine Referenz auf den gespeicherten Wert
	 * zurückgeliefert wird, kann der Operator sowohl als Ziel einer
	 * Zuweisung als auch zum Abfragen des aktuell gespeicherten Wertes
	 * verwendet werden.
	 *
	 * @param position Index der Anzeigestelle (0 für Stelle ganz rechts)
	 * @return Referenz auf den an der ausgewählten Stelle anzuzeigenden Wert
	 */
	uint8_t& operator[] (int position);

	void setDecimalPoint (int digit);

	void setValue(int value);

	CSevenSegmentDisplay& operator= (int value);

	void setValue(float value);
	CSevenSegmentDisplay& operator= (float value);

	/**
	 * The brightness of 7 Segment Display is controlled in between 5% and 100%
	 *
	 * @param percent the brightness percentage
	 */
	void setBrightness(int percent);

	/**
	 * Return the current brightness level
	 */
	int getBrightness();

};

#endif /* CSEVENSEGMENTDISPLAY_H_ */
