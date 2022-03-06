/*
 * CShiftRegisterOutputExtender.h
 *
 *  Created on: 17.10.2020
 *      Author: mnl
 */

#ifndef CSHIFTREGISTEROUTPUTEXTENDER_H_
#define CSHIFTREGISTEROUTPUTEXTENDER_H_

#include "mbed.h"

/**
 * Model which creates an additional output through a 8 bit buffer shift register
 */
class CShiftRegisterOutputExtender {
private:
	DigitalOut* m_serialData;
	DigitalOut* m_shiftRegisterClock;
	DigitalOut* m_bufferRegisterClock;
	uint8_t m_preparedOutput;
	uint8_t m_currentOutput;

public:
	/**
	 * Creates a new object
	 *
	 * @param serialData Pointer at the output, which is with the Data input of Shift Register connected
	 *
	 * @param shiftRegisterClock Pointer at the output, which is with the Clock input of Shift Register connected
	 *
	 * @param bufferRegisterClock Pointer at the output, which is with the Clock input of the buffer Register connected
	 */
	CShiftRegisterOutputExtender(DigitalOut* serialData,
			DigitalOut* shiftRegisterClock, DigitalOut* bufferRegisterClock);

	/**
	 * Prepares the output for the given value in the parameter at the output of shift register.
	 * The Method shifts the values in the shift register but not yet in the buffer register.
	 *
	 * Bit 7 of the passed value should be Output to QH , Bit 6
	 * at QG etc. Bit 0 is given at QA.
	 *
	 * @param value the value to be passed in the shift register
	 */
	void prepareOutput(uint8_t value);

	/**
	 * The value passed from the shift register to the buffer register and
	 * the output is updated to be displayed
	 */
	void updateOutput();

	/**
	 * The value given in the parameter is shown as the output.
	 * The Methods combines the calling of the Method prepareOutput() and updateOutput()
	 *
	 * @param value the output value
	 */
	void write(uint8_t value);

	/**
	 * Operator overloading of the Method write()
	 *
	 * @param value the output value
	 */
	CShiftRegisterOutputExtender& operator= (uint8_t value);

	/**
	 * Returns the value which is currently at output
	 *
	 * @return the value
	 */
	uint8_t read() const;

	/**
	 * Cast-Operator for the Method read()
	 *
	 * @return the value which is currently at output
	 */
	operator int () const;
};

#endif /* CSHIFTREGISTEROUTPUTEXTENDER_H_ */
