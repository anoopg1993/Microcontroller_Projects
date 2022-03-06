#include "mbed.h"
#include "breadboard.h"
#include "CPolledTimer.h"
#include "CDebouncer.h"

#include "stdint.h"

// Legen Sie die globalen Objekte, die die Peripheriekomponenten
// repräsentieren, hier an (vor den Funktionen).

// DigitalOut led1(LED1);

BusOut leds(BB_LED_0,BB_LED_1,BB_LED_2,BB_LED_3,BB_LED_4 ,BB_LED_5 ,BB_LED_6 ,BB_LED_7);

BusIn keys(BB_BTN_0,BB_BTN_1,BB_BTN_2,BB_BTN_3,BB_BTN_4 ,BB_BTN_5 ,BB_BTN_6 ,BB_BTN_7);


/**
 * Blinking of all LEDS at gap of 100 ms
 *
 * The BusOut Object takes Integer as a parameter.The integer value gives the corresponding
 * DigitalOut pin value of High or Low.
 * The =operator is overloaded and is short form for calling the function void write(int value).
 * thread_sleep_for() Function deactivates the performed activity for the time
 * in milliseconds given in the parameter.
 */
void task1() {

	while (true) {
		leds=255;                           //switch on all LEDS
		thread_sleep_for(100);              //wait for 100 ms
		leds = 0;                           //switch off all LEDS
		thread_sleep_for(100);              //wait for 100 ms
	}
}

//***********************************************************************************************************************************************
/**
 * LED lights as long as Button is pressed
 *
 * The read() function of BusIn returns an Integer value, which is the value
 * of pressed button corresponding to the value of DigitalIn Pin.
 * The returned value is assigned to the BusOut Object leds.
 */
void task2() {

	while (true) {
		leds = keys.read();                //Read the value from buttons and switch on LEDs
	}
}

//***********************************************************************************************************************************************
/**
 * Toggle of LEDS
 *
 * The read() function of BusIn returns an Integer value, which is the value
 * of pressed button corresponding to the value of DigitalIn Pin.
 * The returned value is assigned to the newKeyStates.
 * Then xor is carried out between previousKeyStates and newKeyStates.
 */

void task3()
{
	    uint8_t previousKeyStates = 0;
		uint8_t newKeyStates = 0;
		leds = 0;
		previousKeyStates = keys.read();
		while(true){
			newKeyStates = keys;
			leds = leds^(newKeyStates&(~previousKeyStates));
			previousKeyStates = newKeyStates;
		}

}
//***********************************************************************************************************************************************
/**
 * LEDs toggeln with a Debouncer (no value read during the wait time)
 *
 * The class CDebouncer is a decorator to the object of type BusIn.
 * The read() function of the class CDebouncer will not read the value during
 * the wait time given as a parameter.
 */
void task4()
{
	    CDebouncer debounceKeys(&keys,100);
		uint8_t previousKeyStates = 0;
		uint8_t newKeyStates = 0;
		leds = 0;
		previousKeyStates = debounceKeys.read();
		while (true){
			newKeyStates = debounceKeys.read();
			leds = leds^(newKeyStates&(~previousKeyStates));
			previousKeyStates = newKeyStates;
		}

}

//***********************************************************************************************************************************************
/**
 * LEDS lightning one by one from right to left (Lauflicht)
 *
 * The high pins shift from right to left using left shifting operator
 * with a certain wait time 1000ms = 1 s.
 */
void task5()
{

	while(true)
	{
		leds=0;
		for(int shiftIndex=0;shiftIndex<8; shiftIndex++ )
		{
			leds = 1<<shiftIndex;            //Left shift of value 1
			thread_sleep_for(1000);
		}
	}
}

//***********************************************************************************************************************************************
/**
 *LEDS lightning one by one from right to left with waitTime (Lauflicht)
 *
 *Checks every time if the targetTime reached. When the condition true, shifts the LedState.
 */

void task6()
{
	//Initialize values

	uint8_t ledState = 0;
	int64_t currentTime;
	int64_t waitTime;
	int64_t targetTime;
	waitTime = 1000;
	uint8_t shiftIndex = 0;

	//Target time calculation
	currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			HighResClock::now().time_since_epoch()).count();
	targetTime = currentTime + waitTime;

	while(true)
	{
		//Target time calculation
		currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
				HighResClock::now().time_since_epoch()).count();

		if (currentTime >= targetTime)
		{
			targetTime += waitTime;                   //Targettime erhöhen
			ledState = 1<< shiftIndex ;
			shiftIndex++;
			if (shiftIndex==8)                       //Restart lightning from LED 1
				{
				shiftIndex = 0;
				}
		}

		//Assign the value of the new LED states to the LED Bus
		leds = ledState;

	}

}

//***********************************************************************************************************************************************
/**
 * Leds blink at different Frequency
 *
 * The led(0), led(1) und led(2) blink each at different frequency.
 * The method timeReached() checks after every iteration, if targetTime reached.
 * When condition is true, the corresponding Led is toggled which creates a blinking effect.
 */

void task7() {
	CPolledTimer twoHz(500);
	CPolledTimer threeHz(333);
	CPolledTimer fourHz(250);
	while (true) {
		if (twoHz.timeReached()) {
			leds = leds ^ (1 << 0);
		}
		if (threeHz.timeReached()) {
			leds = leds ^ (1 << 1);
		}
		if (fourHz.timeReached()) {
			leds = leds ^ (1 << 2);
		}
	}
}

//***********************************************************************************************************************************************
/**
 * Task 6 (Lauflicht) using CPolledTimer which substitutes targetTime and waitTime in a class
 */

void task8() {
	//Initialize values

	uint8_t shiftIndex = 0;
	CPolledTimer chasingLightTimer(500);

	while(true)
	{
		leds = 1<<shiftIndex;
		if (chasingLightTimer.timeReached())          //Polling Method
		{
			shiftIndex++;
		}

		if(shiftIndex==8)
		{
			shiftIndex=0;
		}
	}
}
//***********************************************************************************************************************************************
/**
 *Configurable running Light
 *
 *The running lights are controlled in two ways:-
 *1. The speed is given directly from user.
 *2. The keys +, - and <-> are used.
 */

void task9(){
    int8_t controlVariable = 0;
	CDebouncer keysDebounced (&keys, 50);
	uint8_t shiftIndex = 0;
	CPolledTimer chasingLightTimer(1000);
	uint8_t previousKeyStates, newKeyStates;
	bool toggledByUser;

	previousKeyStates = keysDebounced;

	while (true) {

		//Functionality for +, - and <-> Buttons. The value should saturate within the range [-7,7]
		toggledByUser = false;
		newKeyStates = keysDebounced;

		if (previousKeyStates == newKeyStates) {           //Skip if no change in state of keys
			continue;
		}
		else if (newKeyStates == 16) {                   // 16 = 00010000 (Addition button pressed)
			controlVariable++;                            //Increase the speed by 1
			if (controlVariable == 8) {                   //If speed reaches 8 then make it 7 (max value)
				controlVariable--;
			}
		}
		else if (newKeyStates == 32) {                   //32 =00100000 (Subtraction button  pressed)
			controlVariable--;                            //Decrease the speed by 1
			if (controlVariable == -8) {                  //If speed reaches -8 then make it -7 (min value)
				controlVariable++;
			}
		}
		else if (newKeyStates == 64) {                   //64 = 01000000 (Change Direction button pressed)
			controlVariable = -controlVariable;           //Negate the speed
		}

		// Speed given directly by the user
		if ((newKeyStates & 15) != 0) {
			leds = leds ^ (newKeyStates & 15);
			toggledByUser = true;
		}
		if (toggledByUser == true) {
			controlVariable = leds & 7;                  //Bit 1, 2 and 3 give the speed

			// Reference on 2s complement table : http://programmingansic.blogspot.com/2014/11/1s-and-2s-complement.html
			if ((leds & (1 << 3)) != 0) {                //Check if the fourth bit is 1. If 1,then speed negative.
				controlVariable = (~controlVariable + 1) & 0x07;                    //2s Complement
				if (controlVariable == - 8) {            //Error Handling
					controlVariable = 0;
				}
			}
		}

		if (controlVariable == 0) {                      //Speed is 0, no LEDs should light
			leds = 0;
		}
		else if (controlVariable < 0) {                  //Speed is negative
			leds = leds | 0b00001000;
			leds = leds & 0b11111000;
			leds = leds | (controlVariable & 0b00000111);

		}
		else if (controlVariable > 0) {                  //Speed is positive
			leds = leds & 0b11110111;
			leds = leds & 0b11110000;
			leds = leds | (controlVariable & 0b00000111);

		}
		previousKeyStates = newKeyStates;

		//Varying the speed of running light based on the speed set by user
		chasingLightTimer.updateWaitTime(abs(controlVariable) * 250);

		do {
			if (controlVariable == 0) {
				leds = 0;
			}

			//Speed is positive, LEDS move from left to right
			else if (controlVariable > 0 && controlVariable <=7) {
				if (chasingLightTimer.timeReached()) {
					leds = leds & 0b00001111;
					leds = leds ^ (128 >> shiftIndex);      //128 =10000000
					shiftIndex++;
					if (shiftIndex == 4){
						shiftIndex = 0;
					}
				}
			}

			//Speed is negative, LEDS move from right to left
			else if (controlVariable < 0) {
				if (chasingLightTimer.timeReached()) {
					leds = leds & 0b00001111;
					leds = leds ^ (16 << shiftIndex);       //16=00010000
					shiftIndex++;
					if (shiftIndex == 4){
						shiftIndex = 0;
					}
				}
			}
		} while (keysDebounced == 0);
	}

}

    /*void task9(){
    int8_t controlVariable = 0;
	CDebouncer keysDebounced (&keys, 50);
	uint8_t shiftIndex = 0;
	CPolledTimer chasingLightTimer;
	uint8_t previousKeyStates, newKeyStates;
	bool toggledByUser;

	previousKeyStates = keysDebounced;

	while (true) {

		//Functionality for +, - and <-> Buttons. The value should saturate within the range [-7,7]
		toggledByUser = false;
		newKeyStates = keysDebounced;
		if (previousKeyStates == newKeyStates) {           //Skip if no change in state of keys
			continue;
		}
		else if (keysDebounced == 16) {                   // 16 = 00010000 (Addition button pressed)
			controlVariable++;                            //Increase the speed by 1
			if (controlVariable == 8) {                   //If speed reaches 8 then make it 7 (max value)
				controlVariable--;
			}
		}
		else if (keysDebounced == 32) {                   //32 =00100000 (Subtraction button  pressed)
			controlVariable--;                            //Decrease the speed by 1
			if (controlVariable == -8) {                  //If speed reaches -8 then make it -7 (min value)
				controlVariable++;
			}
		}
		else if (keysDebounced == 64) {                   //64 = 01000000 (Change Direction button pressed)
			controlVariable = -controlVariable;           //Negate the speed
		}

		// Speed given directly by the user
		if ((keysDebounced & 15) != 0) {
			leds = leds ^ (newKeyStates & 15);
			toggledByUser = true;
		}
		if (toggledByUser == true) {
			controlVariable = leds & 7;                  //Bit 1, 2 and 3 give the speed

			// Reference on 2s complement table : http://programmingansic.blogspot.com/2014/11/1s-and-2s-complement.html
			if ((leds & (1 << 3)) != 0) {                //Check if the fourth bit is 1. If 1,then speed negative.
				controlVariable -= 8;                    //2s Complement
				if (controlVariable == - 8) {            //Error Handling
					controlVariable = 0;
				}
			}
		}
		if (controlVariable == 0) {                      //Speed is 0, no LEDs should light
			leds = 0;
		}
		else if (controlVariable < 0) {                  //Speed is negative
			leds = leds | 0b00001000;
//			leds = leds & 0b11111000;
			leds = leds | (controlVariable & 0b00000111);
		}
		else if (controlVariable > 0) {                  //Speed is positive
			leds = leds & 0b11110111;
//			leds = leds & 0b11110000;
			leds = leds | (controlVariable & 0b00000111);
		}
		previousKeyStates = newKeyStates;

		//Varying the speed of running light based on the speed set by user
		chasingLightTimer.updateWaitTime(abs(controlVariable) * 250);

		do {
			if (controlVariable == 0) {
				leds = 0;
			}

			//Speed is positive, LEDS move from left to right
			else if (controlVariable > 0 && controlVariable <=7) {
				if (chasingLightTimer.timeReached()) {
					leds = leds & 0b00001111;
					leds = leds ^ (128 >> shiftIndex);      //128 =10000000
					shiftIndex++;
					if (shiftIndex == 4){
						shiftIndex = 0;
					}
				}
			}

			//Speed is negative, LEDS move from right to left
			else if (controlVariable < 0) {
				if (chasingLightTimer.timeReached()) {
					leds = leds & 0b00001111;
					leds = leds ^ (16 << shiftIndex);       //16=00010000
					shiftIndex++;
					if (shiftIndex == 4){
						shiftIndex = 0;
					}
				}
			}
		} while (keysDebounced == 0);
	}
}
*/


// Legen Sie weitere Funktionen taskN() (mit N = 3, 4, 5, ...) nach Bedarf an.

/**
 * Haupt- oder Einstiegsfunktion des Programms. Wird beim Start nach der
 * Initialisierung der globalen Variablen (bei Objekten: Aufruf des
 * Konstruktors) ausgeführt.
 */
int main() {

	keys.mode(PullDown);                    //activate PullDown resistor
	// task1();
	// task2();
	 //task3();
	 //task4();
	// task5();
	// task6();
	// task7();
	// task8();
	task9();

	/*
	 * Normalerweise endet der Aufruf einer der Funktionen task1(), task2(),
	 * etc. nie, d.h. der nachfolgende Code wird nie ausgeführt. Für den
	 * Fall, dass Ihnen bei der Implementierung einer der Funktionen ein
	 * Fehler unterläuft, ist hier "sicherheitshalber" noch mal eine
	 * Endlossschleife.
	 */
    while (true) {
    }
}

