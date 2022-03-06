/*
 * CPwmTest1.cpp
 */

#include "CPwmGenerator1.h"

#include "mbed.h"

void CPwmGenerator1::configureGpioPin() {
	// GPIO-Pin PC6 of alternate function 2 (10 in binary) configured
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);
	MODIFY_REG(GPIOC->AFR[0], GPIO_AFRL_AFSEL6,
			GPIO_AF2_TIM3 << GPIO_AFRL_AFSEL6_Pos);
}

void CPwmGenerator1::configureTimer() {

	//Prescaler output at calculated value of approx. 2048 Hz enabled
	// 84 *10^6 / 41016 = 2048   (84 MHz systemtakt)
	TIM3->PSC = 41016 - 1;
	TIM3->ARR = 16 * 2048 - 1;

	// Compare register 1 is at 10 * 2048 set
	// PWM1 (Mode 1) configuration. Check pg 363. The value 6-> 110 in binary
	// the output at OC1REF enabled. Check pg 367
	TIM3->CCR1 = 10 << 11;
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

	// ( Update-Event zum Aktualisieren der gepufferten Register erzwingen,
	// das dadurch im SR gesetztes UIF löschen und Timer starten)
	// Re-initialize the counter and generate an update of the registers.
	// UG bit can be set by software, it is automatically cleared by hardware. pg 361 in Ref Manual
	// UIF bit is set by hardware on an update event. It is cleared by software. pg 360 in Ref Manual
	// CEN counter enable and cleared automatically in one-pulse mode, when an update event occurs.pg 354.

	SET_BIT(TIM3->EGR, TIM_EGR_UG);
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

CPwmGenerator1::CPwmGenerator1() {
	// Timer mit Takt versorgen, sonst ist eine Konfiguration nicht möglich.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Verwendete Peripheriegeräte konfigurieren.
	configureGpioPin();
	configureTimer();
}

