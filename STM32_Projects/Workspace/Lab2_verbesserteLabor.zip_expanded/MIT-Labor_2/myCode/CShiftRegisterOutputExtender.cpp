/*
 * CShiftRegisterOutputExtender.cpp
 *
 *  Created on: 17.10.2020
 *      Author: mnl
 */

/*
 * Implementation of the class CShiftRegisterOutputExtender as per the instructions
 * in the Preparation of Lab
 */

#include "CShiftRegisterOutputExtender.h"

CShiftRegisterOutputExtender::CShiftRegisterOutputExtender(
		DigitalOut *serialData, DigitalOut *shiftRegisterClock,
		DigitalOut *bufferRegisterClock) {

	m_serialData = serialData;
	m_shiftRegisterClock = shiftRegisterClock;
	m_bufferRegisterClock = bufferRegisterClock;
}


//Write the value in the shift register
void CShiftRegisterOutputExtender::prepareOutput(uint8_t value)
{
	// Schieben Sie den übergebenen Wert durch Ansteuern
	// von m_serialData und m_shiftRegisterClock in einer
	// Schleife (!) in das Schieberegister.
	// Speichern Sie den Wert außerdem in m_preparedOutput, damit
	// Sie ihn in updateOutput nach m_currentOutput übernehmen können.
	// Schieben Sie den Wert so in das Schieberegister, dass das
	// höchstwertige Bit (b7) später an QH ausgegeben wird und das
	// niederwertigste Bit (b0) an QA.

	int8_t shiftIndex;
	m_preparedOutput = value;
	for (shiftIndex = 7; shiftIndex >= 0; shiftIndex--)
	{
		//*m_serialData = ((value & (1 << shiftIndex)) >> shiftIndex);
		*m_serialData = ((value & (1 << shiftIndex)));
		*m_shiftRegisterClock = 1;
		*m_shiftRegisterClock = 0;
	}

}

//Write the values in the buffer register
void CShiftRegisterOutputExtender::updateOutput() {
	// Übernehmen Sie den Wert im Schieberegister durch Ansteuern
	// von m_bufferRegisterClock an die Ausgänge.
	// Aktualisieren Sie m_currentOutput;

	*m_bufferRegisterClock = 1;                              //Toggle the Clock
	m_currentOutput = m_preparedOutput;
	*m_bufferRegisterClock = 0;

}

//Write the values in both the registers
void CShiftRegisterOutputExtender::write(uint8_t value) {
	this->prepareOutput(value);
	this->updateOutput();
}

//Operator Overloading for the method write(uint8_t value)
CShiftRegisterOutputExtender& CShiftRegisterOutputExtender::operator =(
		uint8_t value) {
	this->write(value);
	return *this;
}

//Read the values from the output of Register
uint8_t CShiftRegisterOutputExtender::read() const {
	return m_currentOutput;
}

//Operator Overloading for the method read()
CShiftRegisterOutputExtender::operator int() const {
	return read();
}
