/*
 * CSevenSegmentDisplay.cpp
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#include "CSevenSegmentDisplay.h"

// Hier sind die Werte aus der Vorbereitung erstellten Array mit Segment-Mustern eingetragen.
uint8_t CSevenSegmentDisplay::patterns[] = { 0xD7, 0x11, 0x8F, 0x9B, 0x59, 0xDA,
		0xDE, 0x91, 0xDF, 0xDB, 0xDD, 0x5E, 0xC6, 0x1F, 0xCE, 0xCC};

CSevenSegmentDisplay::CSevenSegmentDisplay(
		CShiftRegisterOutputExtender *ssdSegments, DigitalOut* digits[4]) {
	// Übergeben Werte in Attribten speichern
	m_ssdSegments = ssdSegments;
	for (int digit = 0; digit < 4; digit++) {
		m_digits[digit] = digits[digit];
	}

	// Array mit anzuzeigenden Ziffern initialisieren
	for (int digit = 0; digit < 4; digit++) {
		m_values[digit] = 0;
	}

	// Als Startzustand wird Stelle 0 aktiviert und alle Segmente werden
	// ausgeschaltet.
	*m_digits[0] = 1;
	for (int digit = 1; digit < 4; digit++) {
		*m_digits[digit] = 0;
	}
	m_activeDigit = 0;
	*m_ssdSegments = 0xff;
}

void CSevenSegmentDisplay::activateNextDigit() {
	// Implementieren Sie hier den in der Vorbereitung am Ende
	// von Abschnitt 2.3 "Optimierung der Anzahl benötigter uC-Ausgänge"
	// beschriebenen Algorithmus zum Aktivieren der nächsten Anzeigestelle
	if(m_pointValue != (m_activeDigit+1)%4){                              //Das „Segment-Muster“ für die Anzeigestelle „(n + 1) % 4“ wird
			m_ssdSegments->prepareOutput                                  //in das Schieberegister übertragen.
			(~patterns[m_values[(m_activeDigit + 1)%4]]);
		}
		else{
			m_ssdSegments->prepareOutput
			(~(patterns[m_values[(m_activeDigit + 1)%4]]| 32));           //Set the decimal point
		}
		*m_digits[m_activeDigit] = 0;                                     //Die Anzeigestelle „n“ wird ausgeschaltet.
		m_activeDigit = (m_activeDigit + 1)%4;                            // „n“ wird auf den Wert „(n + 1) % 4“ gesetzt
		m_ssdSegments->updateOutput();                                    //Die Werte für das neue „Segment-Muster“ werden in das Pufferregister übernommen
		*m_digits[m_activeDigit] = 1;                                     //Die Anzeigestelle „n“ wird eingeschaltet
}

uint8_t& CSevenSegmentDisplay::operator[] (int position) {
	static uint8_t dummy = 0;
	if (position < 0 || position > 3) {
		return dummy;
	}
	return m_values[position];
}

void CSevenSegmentDisplay::setDecimalPoint(int digit){
	if(digit<4 && digit>-1)
	{
	m_pointValue = 3 - digit;
	}

	else{
		m_pointValue =-1;
	}
	/*switch(digit){
	case 0: m_pointValue = 3;
	break;
	case 1: m_pointValue = 2;
	break;
	case 2: m_pointValue = 1;
	break;
	case 3: m_pointValue = 0;
	break;
	}*/
}

void CSevenSegmentDisplay::setValue(int value){
	if(value > 9999 || value < 0){
		for(int digit = 0; digit < 4; digit++){
					m_values[digit] = 14;                               //Display EEEE
				}
	}
	else{
		for(int digit = 3; digit >= 0; digit--){
					m_values[digit] = value%10;
					value/=10;
					}
	}
}

CSevenSegmentDisplay& CSevenSegmentDisplay::operator= (int value){
	setValue(value);
	return *this;
}


//Implementation of the method setValue with a float value
void CSevenSegmentDisplay::setValue(float value){
	int pointLocation = 0;
	uint8_t counter = 0;
	int integerTempValue;
	float tempValue = value;
	pointLocation = value;
	while(pointLocation != 0){
		pointLocation /= 10;
		counter++;
	}
	//m_pointValue = 4-counter;
	switch(counter){
	case 4:
		m_pointValue = 0;
		break;
	case 3:
		m_pointValue = 1;
		break;
	case 2:
		m_pointValue = 2;
		break;
	case 1:
		m_pointValue = 3;
		break;
	case 0:
		m_pointValue = 3;
		break;
	}
	tempValue = tempValue*pow(10,m_pointValue);
	integerTempValue=tempValue;                                 //Float in Integer casted bcoz modulo operator works only with Integer
	setDecimalPoint(m_pointValue);
	setValue(integerTempValue);
	/*if((integerTempValue < 0) || (integerTempValue > 9999)){    //Value negative or greater than 9999
		setDecimalPoint(0);

		for(int digit = 0; digit < 4; digit++){
			m_values[digit] = 14;                               //Display EEEE
		}
	}
	else{
		for(int digit = 3; digit >= 0; digit--){
			m_values[digit] = integerTempValue%10;
			integerTempValue/=10;
		}
	}*/
}

CSevenSegmentDisplay& CSevenSegmentDisplay::operator= (float value){
	setValue(value);
	return *this;
}
