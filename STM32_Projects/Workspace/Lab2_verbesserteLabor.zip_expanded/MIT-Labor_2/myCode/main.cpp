#include "mbed.h"

#include "breadboard.h"
#include "CShiftRegisterOutputExtender.h"
#include "CDebouncer.h"
#include "CSevenSegmentDisplay.h"
#include "CPolledTimer.h"
#include "CMovingAverageFilter.h"
#include "CExponentialFilter.h"

/*
 * Author : Vrinda Prakash Saraf
 */

// Legen Sie die globalen Objekte, die die Peripheriekomponenten
// repräsentieren, hier an (vor den Funktionen).

// DigitalOut led1(LED1);

DigitalOut ssdSerialData(SSD_SER);
DigitalOut ssdShiftRegisterClock(SSD_SCLK);
DigitalOut ssdBufferRegisterClock(SSD_RCLK);

DigitalOut ssdDigit0(SSD_DIGIT_0, OpenDrainNoPull);
DigitalOut ssdDigit1(SSD_DIGIT_1, OpenDrainNoPull);
DigitalOut ssdDigit2(SSD_DIGIT_2, OpenDrainNoPull);
DigitalOut ssdDigit3(SSD_DIGIT_3, OpenDrainNoPull);

BusOut leds(BB_LED_0,BB_LED_1,BB_LED_2,BB_LED_3,BB_LED_4 ,BB_LED_5 ,BB_LED_6 ,BB_LED_7);

BusIn keys(BB_BTN_0,BB_BTN_1,BB_BTN_2,BB_BTN_3,BB_BTN_4 ,BB_BTN_5 ,BB_BTN_6 ,BB_BTN_7);

CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
				&ssdShiftRegisterClock, &ssdBufferRegisterClock);

CDebouncer debouncedKeys(&keys, 50);

DigitalOut* ssdDigitsArr[4] = {&ssdDigit0, &ssdDigit1, &ssdDigit2, &ssdDigit3};

/*
 * Segment lights as per the corresponding button pressed
 * The segment 2 and 3 are switched on.
 */
void task1() {

    ssdDigit0 = 0;
    ssdDigit1 = 1;
    ssdDigit2 = 1;
    ssdDigit3 = 0;

    uint8_t keyStates;
			while (true) {
				keyStates = ~keys.read();
				ssdSegments = keyStates;
			}

}
//***********************************************************************************************************************************************
/*
 * Hexadecimal display on the segments
 *
 */
void task2() {
	uint8_t patterns[16];             //The index of array patterns will light the segment of the corresponding digit in hexadecimal
	patterns[0] = 0b11010111;
	patterns[1] = 0b00010001;
	patterns[2] = 0b10001111;
	patterns[3] = 0b10011011;
	patterns[4] = 0b01011001;
	patterns[5] = 0b11011010;
	patterns[6] = 0b11011110;
	patterns[7] = 0b10010001;
	patterns[8] = 0b11011111;
	patterns[9] = 0b11011011;
	patterns[10] = 0b11011101;
	patterns[11] = 0b01011110;
	patterns[12] = 0b11000110;
	patterns[13] = 0b00011111;
	patterns[14] = 0b11001110;
	patterns[15] = 0b11001100;

	ssdDigit0 = 1; //Segment 0 is active
	ssdDigit1 = 0; //Segment 1 is active
	ssdDigit2 = 0; //Segment 2 is active
	ssdDigit3 = 0; //Segment 2 is active

	uint8_t previousKeyStates = debouncedKeys;
	uint8_t newKeyStates;
	while (true) {
		newKeyStates = debouncedKeys;                           //Read the pressed buttons
		leds = leds^(newKeyStates&(~previousKeyStates));        // Toggle the State
		ssdSegments = ~patterns[leds & 15];                     //The defined patterns describe the ssdSegments
		previousKeyStates=newKeyStates;
	}
}

//***********************************************************************************************************************************************
/*
 * Last four digits of Matrikelnumber on the 7 Segment Display
 * Implementation of Decimal point with 3(leftmost) & 0(rightmost).
 *
 */
void task3(){
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);

	CPolledTimer digitTimer(5);                                 //Taktperiod of 5ms

	//Last four digits of Matrikelnummer
	ssd[0] = 8;
	ssd[1] = 0;
	ssd[2] = 4;
	ssd[3] = 3;

	ssd.setDecimalPoint(3);                                    //The left most decimal point lights
	while(true){
		if(digitTimer.timeReached()){
			ssd.activateNextDigit();
		}
	}
}

//***********************************************************************************************************************************************
/*
 * Display Floating point number
 *
 * Initially a very small value is taken , which multiplies every 1 sec by power 10.
 * When the value is greater than 9999, then EEEE is displayed.
 */
void task4(){
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);
	CPolledTimer digitTimer(5);
	CPolledTimer showNext(1000);                       //1000ms = 1 sekunde
	float value = 0.0001234;
	ssd = value;
	while(true){
		if(digitTimer.timeReached()){
					ssd.activateNextDigit();
					if(showNext.timeReached()){
						if(value==12340){
							value = 0.0001234;
						}else{
							value *= 10;
						}
						ssd = value;
					}
				}
	}
}

//Self implemented task to check setValue(int value)
void task8(){
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);
	CPolledTimer digitTimer(5);
	CPolledTimer showNext(1000);                       //1000ms = 1 sekunde
	int value = 4;
	ssd = value;
	while(true){
			if(digitTimer.timeReached()){
						ssd.activateNextDigit();
						if(showNext.timeReached()){
							if(value==4000){
								value = 4;
							}else{
								value *= 10;
							}
							ssd = value;
						}
					}
		}
}
//***********************************************************************************************************************************************
/*
 * Display Voltage from the Potentiometer
 *
 * The voltage changed with the help of  rotation of Poti.
 * The last two digits of decimal are not stable.
 */
void task5(){
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);
	CPolledTimer digitTimer(5);
	AnalogIn poti(BB_POTI,3.3f);
	CPolledTimer measurementTimer(20);                   //Taktperiode von 20 ms
	ADC1->HTR = 0xFFF * 3/4;
	ADC1->LTR = 0xFFF * 1/4;
//	ADC1->HTR = 0xbff;
//	ADC1->LTR = 0x3ff;
	ADC1->CR1 |= ADC_CR1_AWDEN;
	while(true){
		if(digitTimer.timeReached()){
			ssd.activateNextDigit();
		}
		if(measurementTimer.timeReached()){
			ssd = poti.read_voltage();
			if(ADC1->SR & ADC_SR_AWD){                   //If the status Register bit is set:-
				leds = 1;                                //Then switch on the LED
				ADC1->SR = ADC1->SR & 0;                 //ADC1->SR &= 0; //Reset the status register
			}
			else{
				leds=0;
			}
		}
	}
}

//***********************************************************************************************************************************************
/**
 * Calculate an average value from measured data
 *
 * N values of measured data is taken where the newest value replaces the oldest value.
 * The average value is calculated from the last N measured data.
 * The filtered value is the sum of the all measured data then divided by N.
 */
void task6(){
		CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);
		CPolledTimer digitTimer(5);
		AnalogIn poti(BB_POTI,3.3f);
		CPolledTimer measurementTimer(20);
		CMovingAverageFilter filter(4000);
		ADC1->HTR = 0xFFF * 3/4;                                   //Initialise Watchdog limits
		ADC1->LTR = 0xFFF * 1/4;
//		ADC1->HTR = 0xbff;
//		ADC1->LTR = 0x3ff;
		ADC1->CR1 |= ADC_CR1_AWDEN;
		while(true){
			if(digitTimer.timeReached()){
				ssd.activateNextDigit();
			}
			filter<<poti.read_u16();                               //poti.read_u16 returns not a voltage value,
			                                                       //instead a value between 0 - 0xFFFF.
			if(measurementTimer.timeReached()){
				 uint16_t calculated_average = uint16_t(filter);   //Operator Overloaded function uint16_t() called
				 float calculated_average_volt = (calculated_average/pow(2,16))*3.3f;   //Convert the average into volts
				ssd = calculated_average_volt;
				if(ADC1->SR & ADC_SR_AWD){                         // When the value exceeds HTR or is lower than LTR,
					leds = 1;                                      // then status register bit is set, switch on the led
					ADC1->SR &= 0;                                 //Reset the status bit register
				}
				else{                                              //When the value between HTR and LTR
					leds=0;                                        //the status register bit is off, switch off the led
				}
			}
		}
}
//***********************************************************************************************************************************************
/**
 * Exponential Smoothing of measured data
 *
 * The new filtered value results from the old filtered value plus
 * the difference between new and old value multiplied by factor ɑ.
 */
void task7(){
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);
	CPolledTimer digitTimer(5);
	AnalogIn poti(BB_POTI,3.3f);
	CPolledTimer measurementTimer(20);
	CExponentialFilter filter(0.01);
	ADC1->HTR = 0xFFF * 3/4;
	ADC1->LTR = 0xFFF * 1/4;
	ADC1->CR1 |= ADC_CR1_AWDEN;
	while(true){
		if(digitTimer.timeReached()){
			ssd.activateNextDigit();
		}
		filter<<poti.read_voltage();                                //poti.read_voltage returns voltage value between 0-3.3 V
		if(measurementTimer.timeReached()){
			ssd = float(filter);                                    //Operator Overloaded function float() called

			if(ADC1->SR & ADC_SR_AWD){
				leds = 1;
				ADC1->SR &= 0;
			}
			else{
				leds=0;
			}
		}
	}
}

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgeführt.
 */
int main() {
	// Falls notwendig, ergänzen Sie hier Aufrufe, um Konfigurationen
	// an globalen Objekten vorzunehmen, die nicht durch die Angabe von
	// Argumenten beim Aufruf des Konstruktors vorgenommen werden können.


	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.

	keys.mode(PullDown);

	//task1();
	task2();
	//task3();
	//task4();
	//task5();
	//task6();
	//task7();

	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2()
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgeführt. Für den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterläuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
    while (true) {
    }
}

