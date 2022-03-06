/*
 * main.h
 *
 *  Created on: 20.12.2020
 *      Author: mnl
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <functional>

extern std::function<void()> activeTIM3_IRQHandler;

extern "C" void TIM1_UP_TIM10_IRQHandler();


#endif /* MAIN_H_ */
