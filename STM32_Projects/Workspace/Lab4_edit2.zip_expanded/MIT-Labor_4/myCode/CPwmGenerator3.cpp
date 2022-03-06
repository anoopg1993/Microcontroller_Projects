/*
 * CPwmGenerator3.cpp
 *
 *  Created on: 22-Jan-2022
 *      Author: vrinda
 */

#include "CPwmGenerator3.h"
#include "mbed.h"

void CPwmGenerator3::configureGpioPin() {
	// GPIO-Pin PC6 of alternate function 2 (10 in binary) configured
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);
	MODIFY_REG(GPIOC->AFR[0], GPIO_AFRL_AFSEL6,
			GPIO_AF2_TIM3 << GPIO_AFRL_AFSEL6_Pos);
}

void CPwmGenerator3::configureTimer() {

	//Bei Timer 3 haben Sie nur 32 Bit. Folglich bleibt Ihnen als Takt für den Timer nur 1 MHz.
	//84 MHz / 1MHz = 84
//	TIM3->PSC = m_period;
//	TIM3->ARR = 84 - 1;
	TIM3->PSC= 84-1;  //setzen Sie den Timer Takt auf 1 MHz.

    TIM3->ARR = m_period-1; //stellen Sie dann die Periode ein
	// Initialise the Compare register 1 at 0 through setDutyCycle() Method,
	// PWM1 Configuration. Check pg 363 in Ref. Manual
	// Enable the OC1REF output. Check pg 367 in Ref. Manual
	setDutyCycle(0);
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);

	// Re-initialize the counter and generate an update of the registers.
	// UG bit can be set by software, it is automatically cleared by hardware. pg 361 in Ref Manual
	// UIF bit is set by hardware on an update event. It is cleared by software. pg 360 in Ref Manual
	// CEN counter enable and cleared automatically in one-pulse mode, when an update event occurs.pg 354.
	SET_BIT(TIM3->EGR, TIM_EGR_UG);
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

CPwmGenerator3::CPwmGenerator3(uint16_t period) {

	m_period = period;

	// Provide timer with clock, otherwise a configuration is not possible.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	// Configure the used peripheral device
	configureGpioPin();
	configureTimer();

}

void CPwmGenerator3::setDutyCycle(uint8_t percent){
	// Percentage greater than 100 are treated as 100%
	percent = percent <= 100 ? percent : 100;

	// Multiply the percent with ARR and set output value to capture/compare register 1
	TIM3->CCR1 = ((float) percent / 100 * TIM3->ARR);

}

