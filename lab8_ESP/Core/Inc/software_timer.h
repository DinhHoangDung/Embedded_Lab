/*
 * software_timer.h
 *
 *  Created on: Nov 19, 2024
 *      Author: HOANG DUNG
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include "tim.h"
#include "led_7seg.h"

extern uint16_t flag_timer2;

void timer_init();
void setTimer2(uint16_t duration);
void timer_EnableDelayUs();
void delay_us (uint16_t us);

#endif /* INC_SOFTWARE_TIMER_H_ */
