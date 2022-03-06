/*
 * CSevenSegmentDisplay.cpp
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#include "CSevenSegmentDisplay.h"


// Here are the values from the Vorbereitung created Array with Segment-Pattern in hexadecimal values
uint8_t CSevenSegmentDisplay::patterns[] = { 0xD7, 0x11, 0x8F, 0x9B, 0x59, 0xDA,
		0xDE, 0x91, 0xDF, 0xDB, 0xDD, 0x5E, 0xC6, 0x1F, 0xCE, 0xCC};

CSevenSegmentDisplay* CSevenSegmentDisplay::m_instance;

CSevenSegmentDisplay::CSevenSegmentDisplay(
		CShiftRegisterOutputExtender *ssdSegments, DigitalOut* digits[4]) {
	initTimer();
	m_instance = this;

	// the transfered value saved in the Attribute
	m_ssdSegments = ssdSegments;
	for (int digit = 0; digit < 4; digit++) {
		m_digits[digit] = digits[digit];
	}

	// Initialize the Array with the digit to be displayed
	for (int digit = 0; digit < 4; digit++) {
		m_values[digit] = 0;
	}

	// In the initial state is the position 0 of Segment is on and the rest are off

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
	// The „Segment-Pattern“ for the position in the segment „(n + 1) % 4“
	// and this value is trasferred in the shift register
	if(m_pointValue != (m_activeDigit+1)%4){
		m_ssdSegments->prepareOutput
		(~patterns[m_values[(m_activeDigit + 1)%4]]);
	}
	else{
		m_ssdSegments->prepareOutput
		(~(patterns[m_values[(m_activeDigit + 1)%4]]| 32));           //Set the decimal point
	}

	// the segment display position „n“ is turned off
	// „n“ is of the value from „(n + 1) % 4“ set
	// The value for the new „Segment-pattern“ is taken in the Bufferregister
	// the segment display position „n“ is turned on
	*m_digits[m_activeDigit] = 0;
	m_activeDigit = (m_activeDigit + 1)%4;
	m_ssdSegments->updateOutput();
	*m_digits[m_activeDigit] = 1;
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
	//Float in Integer casted bcoz modulo operator works only with Integer
	integerTempValue=tempValue;
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

void CSevenSegmentDisplay::initTimer() {
	// Provide timer with clock, otherwise a configuration is not possible.
	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;

	/*// Set the values of Prescaler und Auto-Reload
	TIM10->PSC = 8400 - 1; //setzen Sie den Timer Takt auf 1 MHz.
	TIM10->ARR = 50 - 1;  //TIM3->ARR = m_period-1 //stellen Sie dann die Periode ein*/

	TIM10 -> PSC = ((5000)*84)/4096 - 1;
	TIM10 -> ARR = 16 * 256 - 1;
	// PWM1 Configuration. Check pg 414 in Ref. Manual
	// Enable the OC1REF output. Check pg 417 in Ref. Manual
	MODIFY_REG(TIM10->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);
	SET_BIT(TIM10->CCER, TIM_CCER_CC1E);

	// Enable Interrupt
	__NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);            // Enable TIM10-Interrupts
	SET_BIT(TIM10->DIER, TIM_DIER_UIE);      // Set the Update-Interrupt
	SET_BIT(TIM10->DIER, TIM_DIER_CC1IE);    // Set the CC1(Capture/Compare)

	// Re-initialize the counter and generate an update of the registers.
	// UG bit can be set by software, it is automatically cleared by hardware. pg 414 in Ref Manual
	// UIF bit is set by hardware on an update event. It is cleared by software. pg 413 in Ref Manual
	// CEN counter enabled and cleared automatically in one-pulse mode, when an update event occurs.pg 412.
	SET_BIT(TIM10->EGR, TIM_EGR_UG);
	CLEAR_BIT(TIM10->SR, TIM_SR_UIF);
	SET_BIT(TIM10->CR1, TIM_CR1_CEN);
}

void CSevenSegmentDisplay::setBrightness(int percent) {
	if (percent < 5) {
		percent = 5;
	}
	else if (percent > 100) {
		percent = 100;
	}
	m_brightness = percent;
	TIM10->CCR1 = ((float) percent/100) * TIM10->ARR;
}

int CSevenSegmentDisplay::getBrightness() {
	return m_brightness;
}
void CSevenSegmentDisplay::shutDownAllDigits() {
	for (int digit = 0; digit < 4; digit++) {
		*m_digits[digit] = 0;
	}
}
