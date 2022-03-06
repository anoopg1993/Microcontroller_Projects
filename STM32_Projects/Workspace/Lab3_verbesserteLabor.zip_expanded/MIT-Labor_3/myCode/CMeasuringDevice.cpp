/*
 * CMeasuringDevice.cpp
 *
 *  Created on: 07-Dec-2021
 *      Author: Vrinda Prakash Saraf
 */

#include "CMeasuringDevice.h"

const static int DEBOUNCE_TIME_MS            = 50;
const static int CONTINUOUS_POLL_INTERVAL_MS = 100;
const static float SMOOTHING_FACTOR          = 0.0001;

//Initialise all the variables in the constructor
CMeasuringDevice::CMeasuringDevice(CSevenSegmentDisplay* display, BusIn* keys,
		BusOut* leds, BufferedSerial* usart, AnalogIn* voltageInput) {

	CDebouncer* keysDebounced = new CDebouncer(keys, DEBOUNCE_TIME_MS);
	CPolledTimer* measurementTimerContinuous= new CPolledTimer(CONTINUOUS_POLL_INTERVAL_MS);
	m_filter = new CExponentialFilter(SMOOTHING_FACTOR);

	m_display = display;
	m_keys = keysDebounced;
	m_leds = leds;
	m_usart = usart;
	m_voltageInput = voltageInput;
	m_displayMode = displayMode::BAR;
	m_measureMode = measureMode::CON_STOP;
	m_timer = measurementTimerContinuous;
	bShowOutput = true;
	bIsRepeated = false;
	previousKeyStates = 0;

	char OutputText1[] = "MIT-Digitalvoltmeter : SW-Rev. 0.1\n\r";  //Check wht happens without /r
	// -1 to avoid the terminating Null to transfer in sizeof()
	m_usart->write(OutputText1, sizeof(OutputText1)-1);
}

//Delete the dynamic variables in the destructor
CMeasuringDevice::~CMeasuringDevice() {
	delete m_keys;
	delete m_timer;
	delete m_filter;
}

void CMeasuringDevice::poll() {
	currentKeyStates = *m_keys;
	if (previousKeyStates == currentKeyStates && previousKeyStates != 0
			&& currentKeyStates != 0) {
		bIsRepeated = true;
	}
	else {
		bIsRepeated = false;
	}
	pollKeys();
	pollKeyboard();
	pollWrite();
	previousKeyStates = currentKeyStates;
}

void CMeasuringDevice::pollKeys() {
	if (currentKeyStates != 0 && currentKeyStates >= 16 && !bIsRepeated) {
		setDisplayMode(currentKeyStates);
		OutputTextOnTerminalDisplay();
	}
	else if (currentKeyStates != 0 && currentKeyStates <= 8 && !bIsRepeated) {
		setMeasurementMode(currentKeyStates);
	}
}

void CMeasuringDevice::pollKeyboard() {
	if (m_usart->read(&keyboardInput, 1) == 1) {
		m_usart->read(&keyboardInput, 1);
		switch (keyboardInput) {
		case 'e':
		case 'f':
		case 'h':
		case 's':
			setMeasurementMode(keyboardInput);
			break;
		case 'b':
		case 'c':
		case 'i':
		case 'p':
			setDisplayMode(keyboardInput);
			OutputTextOnTerminalDisplay();
			break;
		case 'v': {
			char OutputText1[] = "MIT-Digitalvoltmeter : SW-Rev. 0.1\n\r";
			m_usart->write(OutputText1, sizeof(OutputText1)-1);
			break;
		}
		default : {
			char errorOutputText[] = "Unbekannter Befehl\n\r";
			m_usart->write(errorOutputText, sizeof(errorOutputText)-1);
			break;
		}
		}
	}
}

void CMeasuringDevice::setDisplayMode(int keys) {
	uint8_t buttonPressed = 0;
	/**
	 * Keep on shifting the value by 1 until the pressed button value of display Mode
	 * from user is reached.The buttonPressed keeps incrementing during the cycle.
	 * The buttonPressed decides the display mode.
	 */
	while (keys > 1) {
		keys = keys >> 1;
		buttonPressed++;
	}
	switch (buttonPressed) {
	case 4:
		m_displayMode = displayMode::PC;
		break;
	case 5:
		m_displayMode = displayMode::BINARY;
		break;
	case 6:
		m_displayMode = displayMode::BCD;
		break;
	case 7:
		m_displayMode = displayMode::BAR;
		break;
	default:
		break;
	}
}

void CMeasuringDevice::setDisplayMode(char keys) {
	switch (keys) {
	case 'p':
		m_displayMode = displayMode::PC;
		break;
	case 'i':
		m_displayMode = displayMode::BINARY;
		break;
	case 'c':
		m_displayMode = displayMode::BCD;
		break;
	case 'b':
		m_displayMode = displayMode::BAR;
		break;
	default:
		break;
	}
}

void CMeasuringDevice::setMeasurementMode(int keys) {
	if (keys == 8) {
		measureSingle();
	}
	else if (keys == 4) {
		measureContinuous();
	}
	else if (keys == 2 && bIsRepeated == false) {
		continuousToggleHold();
	}
	else if (keys == 1) {
		measureStop();
	}
}


void CMeasuringDevice::setMeasurementMode(char keys) {
	if (keys == 'e') {
		measureSingle();
	}
	else if (keys == 'f') {
		measureContinuous();
	}
	else if (keys == 'h' && bIsRepeated == false) {
		continuousToggleHold();
	}
	else if (keys == 's') {
		measureStop();
	}
}

void CMeasuringDevice::measureSingle() {
	if (m_measureMode == measureMode::SINGLE) {
		bShowOutput = true;
	}
	else {
		m_measureMode = measureMode::SINGLE;
		bIsRepeated = false;
	}
	char outputText2[] = "Einzelmessung gestartet\n\r";
	m_usart->write(outputText2, sizeof(outputText2)-1);
}

void CMeasuringDevice::measureContinuous() {
	m_measureMode = measureMode::CONTINUOUS;
	bShowOutput = true;
	bIsRepeated = false;
	char outputText3[] = "Dauermessung gestartet\n\r";
	m_usart->write(outputText3, sizeof(outputText3)-1);
}

void CMeasuringDevice::continuousToggleHold() {
	if (m_measureMode == measureMode::CON_HOLD) {
		m_measureMode = measureMode::CONTINUOUS;
		bShowOutput = true;
		bIsRepeated = true;
		char outputText4[] = "Dauermessung fortgesetzt\n\r";
		m_usart->write(outputText4, sizeof(outputText4)-1);
	}
	else {
		m_measureMode = measureMode::CON_HOLD;
		//bIsRepeated = false;                          //++++++++++++++++++
		char outputText5[] = "Dauermessung pausiert\n\r";
		m_usart->write(outputText5, sizeof(outputText5)-1);
	}
}

void CMeasuringDevice::measureStop() {
	m_measureMode = measureMode::CON_STOP;
	bIsRepeated = false;

	*m_display = 0;
	char outputText6[] = "Dauermessung gestoppt\n\r";
	m_usart->write(outputText6, sizeof(outputText6)-1);
}

void CMeasuringDevice::displayOnLeds(float measuredVoltage) {
	switch (m_displayMode) {
	case displayMode::BAR: {
		displayBar(measuredVoltage);
		break;
	}
	case displayMode::BCD: {
		displayBCD(measuredVoltage);
		break;
	}
	case displayMode::BINARY: {
		displayBinary(measuredVoltage);
		break;
	}
	case displayMode::PC: {
		displayPC(measuredVoltage);
		break;
	}
	default:
		break;
	}
}

void CMeasuringDevice::displayBar(float valueToShow) {

	uint8_t ledLevel = 0;
	//Calculate level of LED
	ledLevel = 9 - (valueToShow/ (3.3/9));
	*m_leds =255<<ledLevel;

	//pow functions should be avoided
	//*m_leds = pow(2,ledLevel)-1;
}

void CMeasuringDevice::displayBCD(float valueToShow) {
	uint8_t decimal, integer;

	integer = int(valueToShow)<<4;
	//modulo doesn't work with decimals because it only accepts integers therefore multiplication
	decimal = valueToShow*10;
	decimal %= 10;
	*m_leds = integer + decimal;

}

void CMeasuringDevice::displayBinary(float valueToShow) {

	uint8_t binaryValue;
	//Basic math calculation to convert the measured voltage into binary 0-255
	binaryValue = ((double)valueToShow/3.3)*255;
	*m_leds = binaryValue;
}

void CMeasuringDevice::displayPC(float valueToShow) {

	*m_leds = 0;
	char data[7];
	char carriageReturn = '\r';
	int intValueToShow;
	int modulo;

	//Convert into Integer
	intValueToShow = valueToShow * 1000;
	for(int i = 4; i>=0; i--){
		if(i == 1){
			data[i] = '.';
		}else{
			modulo = intValueToShow % 10;
			intValueToShow = intValueToShow / 10;
			data[i] = modulo + '0';
		}
	}
	data[5] = ' ';
	data[6] = 'V';
	data[7] = '\r';
	m_usart->write(&carriageReturn, 1);
	m_usart->write(data, 8);

	// Avoid to_string
	/* *m_leds = 0;
	char unit[3] = {' ', 'V', '\r'};
	//to_string converts numerical value to string
	string valueString = to_string(valueToShow);
	//Assign all characters from string into char Array
	char outputVoltage[5];
	for (int shift = 0; shift < 5; shift++) {
		outputVoltage[shift] = valueString[shift];
	}
	m_usart->write(outputVoltage, 5);
	m_usart->write(unit, 3);*/
}

void CMeasuringDevice::OutputTextOnTerminalDisplay() {
	if (m_displayMode == displayMode::PC) {
		char outputText7[] = "Displaymodus: PC\n\r";
		m_usart->write(outputText7, sizeof(outputText7)-1);
	}
	else if (m_displayMode == displayMode::BINARY) {
		char outputText8[] = "Displaymodus: Binär\n\r";
		m_usart->write(outputText8, sizeof(outputText8)-1);
	}
	else if (m_displayMode == displayMode::BCD) {
		char outputText9[] = "Displaymodus: BCD\n\r";
		m_usart->write(outputText9, sizeof(outputText9)-1);
	}
	else if (m_displayMode == displayMode::BAR) {
		char outputText10[] = "Displaymodus: Balken\n\r";
		m_usart->write(outputText10, sizeof(outputText10)-1);
	}
}

void CMeasuringDevice::pollWrite() {
	prevSampledPotVoltage = float(*m_filter<<m_voltageInput -> read_voltage());
	//lastMeasured = m_voltageInput->read_voltage();    //Value read without the filter
	if (m_measureMode == measureMode::CONTINUOUS && m_timer->timeReached() && bShowOutput) {
		displayOnLeds(prevSampledPotVoltage);
		//display the value on 7 segment display
		*m_display = prevSampledPotVoltage;
	}

	else if ((m_measureMode == measureMode::CON_HOLD || m_measureMode == measureMode::SINGLE)
				&& bShowOutput) {
			displayOnLeds(prevSampledPotVoltage) ;
			*m_display = prevSampledPotVoltage;
			bShowOutput = false;
		}

	else if (m_measureMode == measureMode::CON_STOP && bShowOutput) {
		prevSampledPotVoltage = float(*m_filter<< 0);  //++++++
		displayOnLeds(0);
		*m_display = 0;
		bShowOutput = false;
	}


}



