#include "mbed.h"

#include "breadboard.h"
#include "CShiftRegisterOutputExtender.h"
#include"CSevenSegmentDisplay.h"
#include"CPolledTimer.h"
#include"CHexShifter.h"
#include"CMeasuringDevice.h"
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

DigitalOut* ssdDigitsArr[4] = {&ssdDigit0, &ssdDigit1, &ssdDigit2, &ssdDigit3};

AnalogIn poti(BB_POTI,3.3f);

/**
 * Implementierung der Lösung zur Aufgabe 1.
 */
void task1() {
	// Schreiben Sie vor der Enlosschleife den einmal auszuführenden Code

	BufferedSerial usart(PA_2, PA_3, 115200);
    while (true) {
    	// Schreiben Sie in der Endlosschleife den immer wieder auszuführenden
    	// Code
    	char data[] = { '*' };
    	usart.write(data, 1);
    }
}
//***********************************************************************************************************************************************

void task2(){
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);

	CPolledTimer digitTimer(5);                                 //Taktperiod of 5ms

	BufferedSerial usart(PA_2, PA_3, 115200);
	usart.set_blocking(false);                                   //Schnittstelle auf „non blocking I/O"

	CHexShifter application(&ssd, &keys, &leds, &usart);
	while(true){
		if(digitTimer.timeReached()){
			ssd.activateNextDigit();
		}
		application.poll();
	}
}
//***********************************************************************************************************************************************

void task3(){
	CSevenSegmentDisplay ssd(&ssdSegments, ssdDigitsArr);

	CPolledTimer digitTimer(5);                                 //Taktperiod of 5ms

	BufferedSerial usart(PA_2, PA_3, 115200);
	usart.set_blocking(false);                                   //Schnittstelle auf „non blocking I/O"

	CMeasuringDevice application(&ssd, &keys, &leds, &usart,&poti);
	while(true){
		if(digitTimer.timeReached()){
			ssd.activateNextDigit();
		}
		application.poll();
	}
}

// Legen Sie weitere Funktionen taskN() (mit N = 3, 4, 5, ...) nach Bedarf an.

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgeführt.
 */
int main() {
	// Falls notwendig, ergänzen Sie hier Aufrufe, um Konfigurationen
	// an globalen Objekten vorzunehmen, die nicht durch die Angabe von
	// Argumenten beim Aufruf des Konstruktors vorgenommen werden können.
	keys.mode(PullDown);

	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.
	//task1();
	//task2();
	task3();

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

