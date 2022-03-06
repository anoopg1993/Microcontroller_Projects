#include "mbed.h"
#include "main.h"
#include "breadboard.h"
#include "CPwmGenerator1.h"
#include "CPwmGenerator2.h"
#include "CPwmGenerator3.h"
#include "CPwmGenerator4.h"

#include "CExponentialFilter.h"
#include "CSevenSegmentDisplay.h"
#include "CPolledTimer.h"
#include "CDebouncer.h"

// Legen Sie die globalen Objekte, die die Peripheriekomponenten
// repräsentieren, hier an (vor den Funktionen).

// DigitalOut led1(LED1);

BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3,
		BB_BTN_4, BB_BTN_5, BB_BTN_6, BB_BTN_7);

AnalogIn poti(BB_POTI, 3.3);

DigitalOut ssdSerialData(SSD_SER);
DigitalOut ssdShiftRegisterClock(SSD_SCLK);
DigitalOut ssdBufferRegisterClock(SSD_RCLK);

DigitalOut ssdDigit0(SSD_DIGIT_0, OpenDrainNoPull);
DigitalOut ssdDigit1(SSD_DIGIT_1, OpenDrainNoPull);
DigitalOut ssdDigit2(SSD_DIGIT_2, OpenDrainNoPull);
DigitalOut ssdDigit3(SSD_DIGIT_3, OpenDrainNoPull);
/**
 * Generates a PWM-Signal using CPwmGenerator2 on PC6.
 * Further the Bits 15-10 of Counter are displayed on LEDs 5-0
 */
void task1() {
	// Die LEDs werden flexibler verwendet als in den vorherigen Laboren
	// und daher nicht als globales Objekt definiert sondern nach Bedarf
	// in den verschiedenen Tasks.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5);

	CPwmGenerator1 fixedGenerator;

	while (true) {
		// Bits 15-10 of CNT on LEDs 5-0 displayed.
		leds = (TIM3->CNT >> 10) & 0x3f;
	}
}

/**
 * Generates a PWM-Signal using CPwmGenerator2 on PC6.
 * Further the Bits 15-10 of Counter are displayed on LEDs 5-0
 * The duty cycle is controlled using the Potentiometer.
 */
void task2() {

	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5);

	CPwmGenerator2 variableGenerator;
	CExponentialFilter filter(0.001);

	while (true) {
		// Bits 15-10 of CNT on LEDs 5-0 displayed.
		leds = (TIM3->CNT >> 10) & 0x3f;

		//The value is read from the potentiometer between range 0.0-1.0
		//and the value is filtered using the exponential filter
		//The filtered valued is given as a parameter to setDutyCycle
		//to calculate the duty cycle
		filter << poti.read();
		variableGenerator.setDutyCycle((float(filter) * 100));  //Operator Overloading
	}
}

/**
 * Generates a PWM-Signal using CPwmGenerator3 with Interrupts on PC6.
 * The duty cycle is controlled using the Potentiometer, through which
 * we alter the brightness of the LED.
 * The time period of PWM Signal is given as a parameter while calling the CPwmGenerator
 */
void task3() {

	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5);

	// Create a new PWMGenerator3 object with specified period duration and
	// an exponential filter with a specified smoothing factor
	CPwmGenerator3 variableGenerator(20000);
	CExponentialFilter filter(0.001);

	while (true) {
		// Bits 15-10 of CNT on LEDs 5-0 displayed.
		leds = (TIM3->CNT >> 10) & 0x3f;

		//The value is read from the potentiometer between range 0.0-1.0
		//and the value is filtered using the exponential filter
		//The filtered valued is given as a parameter to setDutyCycle
		//to calculate the duty cycle
		filter << poti.read();
		variableGenerator.setDutyCycle((float(filter) * 100));  //Operator Overloading
	}
}

/**
 * Generates a PWM-Signal using CPwmGenerator4 on PC6.
 * The duty cycle is controlled using the Potentiometer, through which
 * we alter the brightness of the LED.
 * The time period of PWM Signal is given as a parameter while calling the CPwmGenerator4
 * The LED 7 is set to 1 when triggering update flag
 * and when triggering Compare flag set to 0
 */
void task4() {

	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3,BB_LED_4, BB_LED_5);

	// Create a new PWMGenerator4 object with specified period duration and
	// an exponential filter with a specified smoothing factor
	CPwmGenerator4 variableGenerator(19999);
	CExponentialFilter filter(0.001);

	// Creates a DigitalOut object which controls the LED7
	// The Adresse of LED7 given as a parameter to generate a PWM-Signal
	DigitalOut LED7 = PC_7;
	variableGenerator.setPwmOut(&LED7);

	while (true) {
		// Interrupt-Request temporarily deactivated to avoid the disturbance in
		// read and write operation from the Poti and LED
		__disable_irq();

		// The value from the Poti is filtered and given as parameter to setDutyCycle()
		// for determining the duty cycle
		// // Bits 15-10 of CNT on LEDs 5-0 displayed.
		filter << poti.read();
		variableGenerator.setDutyCycle((float(filter) * 100));
		leds = (leds & 0xc0) | ((TIM3->CNT >> 10) & 0x3f);

		// Enable the Interrupt-Request
		__enable_irq();
	}
}

extern "C" void TIM1_UP_TIM10_IRQHandler() {
	if(READ_BIT(TIM10->SR, TIM_SR_UIF)) {
		CLEAR_BIT(TIM10->SR, TIM_SR_UIF);
		CSevenSegmentDisplay::m_instance->activateNextDigit();
	}
	if(READ_BIT(TIM10->SR, TIM_SR_CC1IF)) {
		CLEAR_BIT(TIM10->SR, TIM_SR_CC1IF);
		CSevenSegmentDisplay::m_instance->shutDownAllDigits();
	}
}
void task5(){
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,
			&ssdShiftRegisterClock, &ssdBufferRegisterClock);
	DigitalOut* digitsArr[4] = {&ssdDigit0, &ssdDigit1, &ssdDigit2, &ssdDigit3};
	CSevenSegmentDisplay ssd(&ssdSegments, digitsArr);

	DigitalOut led0(BB_LED_0);

	//Last four digits of Matrikelnummer
		ssd[0] = 8;
		ssd[1] = 0;
		ssd[2] = 4;
		ssd[3] = 3;
	ssd =8043;
	ssd.setBrightness(33);

	while(true){
		__disable_irq();
		led0 = 0;
		__WFI();
		led0 = 1;
		__enable_irq();
	}
}




/**
 * Funktion, die der Timer 3 IRQ Handler aufrufen soll. Ermöglicht
 * die Konfiguration unterschiedlicher effektiver IRQ Handler je
 * nach Nutzung des Timers.
 *
 * Die aufzurufende Funktion muss im Rahmen der Konfiguration des
 * Interrupts zugewiesen werden.
 */
function<void()> activeTIM3_IRQHandler;

/**
 * Interrupt-Handler für Timer 3. Der Handler ruft die Funktion auf,
 * die der globalen Variablen activeTIM3_IRQHandler zugewiesen wurde.
 */
extern "C" void TIM3_IRQHandler() {
	activeTIM3_IRQHandler();
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
	keys.mode(PullDown);

	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.
	//task1();
	//task2();
	//task3();
	//task4();
	task5();
	//task6();

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

