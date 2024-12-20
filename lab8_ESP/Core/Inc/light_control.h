/*
 * light_control.h
 *
 *  Created on: Nov 19, 2024
 *      Author: HOANG DUNG
 */

#ifndef INC_LIGHT_CONTROL_H_
#define INC_LIGHT_CONTROL_H_

#include "gpio.h"
#include "uart.h"
#include "button.h"
#include "lcd.h"

extern uint8_t light_status;

void lightProcess();

void test_Esp();

#endif /* INC_LIGHT_CONTROL_H_ */
