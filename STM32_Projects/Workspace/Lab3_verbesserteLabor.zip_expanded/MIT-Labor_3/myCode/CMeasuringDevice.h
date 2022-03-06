/*
 * CMeasurementDevice.h
 *
 *  Created on: 07-Dec-2021
 *      Author: Vrinda Prakash Saraf
 */

#ifndef CMEASURINGDEVICE_H_
#define CMEASURINGDEVICE_H_

#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"
#include "CExponentialFilter.h"

/**
 * Class CMeasuringDevice implements different ways to measure(Once or Continuous)
 * as well as different ways to represent it(Bar, BCD, Binary, PC)
 */
class CMeasuringDevice {

private:
	CSevenSegmentDisplay* m_display;
	CDebouncer* m_keys;
	BusOut* m_leds;
	BufferedSerial* m_usart;
	AnalogIn* m_voltageInput;
	CPolledTimer* m_timer;
	CExponentialFilter* m_filter;

	// the four types of display modes on LED
	enum class displayMode {PC, BINARY, BCD, BAR};

	// the four types of measurement modes
	enum class measureMode {SINGLE, CONTINUOUS, CON_HOLD, CON_STOP};

	// Attribute to save the selected display and measure mode
	displayMode m_displayMode;
	measureMode m_measureMode;

	// the last measured value from the potentiometer
	float prevSampledPotVoltage;

	// the value given from user on buttons
	uint8_t previousKeyStates;
	uint8_t currentKeyStates;

	// the value given from user in keyboard
	char keyboardInput;

	/**
	 * Transfers the calculated measured voltage through serial port
	 *
	 * @param valueToShow the measured voltage
	 */
	void displayPC(float valueToShow);

	/**
	 * Displays the measured Voltage from potentiometer as Binary between 0 to 255
	 *
	 * @param valueToShow the measured voltage
	 */
	void displayBinary(float valueToShow);

	/**
	 * Displays the measured Voltage from potentiometer as "Binary Coded Decimal".
	 * The four left most LEDs represent the integer part and
	 * the four right most LEDs display the decimal places
	 *
	 * @param valueToShow the measured voltage
	 */
	void displayBCD(float valueToShow);

	/**
	 * Displays the measured Voltage from potentiometer as Bar Diagram,
	 * where the left Led means high voltage and right most LED means low voltage
	 *
	 * @param valueToShow the measured voltage
	 */
	void displayBar(float valueToShow);

	/**
	 * Measures the value only ONCE
	 */
	void measureSingle();

	/**
	 * Measures the value CONTINUOUSLY.
	 */
	void measureContinuous();

	/**
	 * STOPS measuring the value and displays 0 of 7 Segment Display
	 */
	void measureStop();

	/**
	 * HOLDS the last measured voltage or if function called again
	 * then starts with CONTINUOUS Measurement
	 */
	void continuousToggleHold();

	/* If True, the output value to be displayed through the serial port */
	bool bShowOutput;

	/* If True, then no Setter-Method called, to prevent changes to reverse */
	bool bIsRepeated;

	/**
	 * Display the measured voltage as per selected Display mode
	 * from the user via Buttons or Keyboard
	 *  4 : PC
	 *  5 : Binary
	 *  6 : BCD
	 *  7 : Bar
	 *
	 *  @param keys the corresponding integer
	 */
	void setDisplayMode(int keys);

	/**
	 * Measure the voltage from Potentiometer as per the selected Measure mode from User
	 *	1 : Stop the measurement
	 *	2 : Hold/Continue the Continuous Measurement
	 *	3 : Start the Continuous Measurement
	 *	4 : Measure the Voltage Once
	 *
	 *	@param keys the corresponding integer
	 */
	void setMeasurementMode(int keys);

	/**
	 * Display the measured voltage as per selected Display mode
	 * from the user via Buttons or Keyboard
	 *  p : PC
	 *  i : Binary
	 *  c : BCD
	 *  b : Bar
	 *
	 *  @param keys the corresponding character
	 */
	void setDisplayMode(char keys);

	/**
	 * Measure the voltage from Potentiometer as per the selected Measure mode from User
	 *	s : Stop the measurement
	 *	h : Hold/Continue the Continuous Measurement
	 *	f : Start the Continuous Measurement
	 *	e : Measure the Voltage Once
	 *
	 *	@param keys the corresponding character
	 */
	void setMeasurementMode(char keys);

	/**
	 * Firstly it checks if any Button has been pressed.
	 * If yes, then checks, if the button pressed corresponds to Display Mode or Measure Mode
	 * and then the respected Set-Method is called
	 */
	void pollKeys();

	/**
	 * Firstly it checks if any character pressed on the keyboard.
	 * If yes, then checks, whether the character is permitted.
	 * If permitted character pressed then the corresponding Set-Method is called.
	 * Otherwise "Unrecognised Character" is displayed via the serial port .
	 */
	void pollKeyboard();

	/**
	 * Reads the filtered voltage value from the potentiometer.
	 * The corresponding DisplayMode set via display()
	 */
	void pollWrite();

	/**
	 * Checks which Display mode is active and gives the respective text
	 * through the serial port.
	 */
	void OutputTextOnTerminalDisplay();

	/**
	 * The measured voltage displayed on LEDs by calling the respective
	 * Modes function
	 *
	 * @param measuredValue the voltage from Potentiometer
	 */
	void displayOnLeds(float measuredVoltage);

public:
	/**
	 * Erzeugt ein neues Exemplar der Anwendung, das die angegebenen
	 * Hardware-Komponenten benutzt.
	 *
	 * Die Taster werden über ein Exemplar von CDebouncer entprellt.
	 * Da die Klasse CDebouncer keinen Default-Konstruktor hat, kann
	 * das Objekt nicht als Attribut definiert werden, sondern muss im
	 * Konstruktor auf dem Heap erzeugt und im Destruktor wieder
	 * freigegeben werden.
	 *
	 * @param display die 7-Segment-Anzeige
	 * @param keys die Eingänge, an die die Taster angeschlossen sind
	 * @param leds die Ausgänge, an die die LEDs angeschlossen sind
	 * @param usart die serielle Schnittstelle
	 * @param voltageInput der Analogeingang
	 */
	CMeasuringDevice(CSevenSegmentDisplay* display, BusIn* keys,
			BusOut* leds, BufferedSerial* usart, AnalogIn* voltageInput);

	/**
	 * Gibt die allokierten Ressourcen wieder frei.
	 */
	~CMeasuringDevice();

	/**
	 * Ruft die privaten poll...-Methoden zur Verarbeitung der
	 * verschiedenen Eingabequellen auf und prüft, ob der Nutzer den Finger
	 * von der Taste losgelassen hat.
	 */
	void poll();
};


#endif /* CMEASURINGDEVICE_H_ */

