/*
 * CHexShifter.cpp
 *
 *  Created on: 05-Dec-2021
 *      Author: vrind
 */

#include"CHexShifter.h"

CHexShifter::CHexShifter(CSevenSegmentDisplay* display, BusIn* keys,
		BusOut* leds, BufferedSerial* usart){

	m_keys  = new CDebouncer(keys, 150);
	m_display =display;
	m_leds = leds;
	m_usart = usart;
}

CHexShifter::~CHexShifter(){
	    delete[] m_keys;
}

void CHexShifter::pollUsart(){
	char data[1];
	if(m_usart -> read(data,1) == 1){
		processChar(*data);
	}
	else{
		return;
	}
}

void CHexShifter::processChar(char c)
{
	uint8_t convertedValue = 0;
	if((c - '0' >= 0) && (c - '0' <= 9)){
		convertedValue = c - '0';
	}
	else if((c - 'a' >= 0) && (c - 'a' <= 5)){
		convertedValue = c - 'a' + 10;
	}
	else if((c - 'A' >= 0) && (c - 'A' <= 5)){
		convertedValue = c - 'A' + 10;
	}
	else{
		return;
	}
	processDigit(convertedValue);
}

void CHexShifter::processDigit(uint8_t value) {
	if(m_insertLeft==false) {
		for(int i = 0; i<=2; i++){
			m_display->operator [](i) = m_display->operator [](i + 1);
		}
		m_display->operator [](3) = value;
	}
	else if(m_insertLeft == true) {
		for(int i = 3; i>=1; i--){
			 m_display->operator [](i) = m_display->operator [](i-1);
		}
		m_display->operator [](0) = value;
	}
	sendDisplayedDigits();
}

void CHexShifter::sendDisplayedDigits() const {
	char data[6];
	for(int i = 0; i<=3; i++) {
		if(m_display->operator [](i) < 10){
			data[i] = m_display->operator [](i) + '0';                //'0'=48
		}
		if(m_display->operator [](i) >= 10){
			data[i] = m_display->operator [](i) + 'a' - 10;           //'a'=97
		}
	}
	data[4] = '\r';                         //0xB
	data[5] = '\n';                         //0xD
	m_usart -> write(data, 6);
}

void CHexShifter::pollKeys() {
	int keys_Masked = 0x1;
	if((m_keys->read() & keys_Masked) == 1){
		m_insertLeft = !m_insertLeft;
		thread_sleep_for(200);
	}
	if(m_insertLeft==true){
		m_leds -> write(0x1);
	}
	else {
		m_leds -> write(0x0);
	}
}

void CHexShifter::poll() {
	pollUsart();
	pollKeys();
}
