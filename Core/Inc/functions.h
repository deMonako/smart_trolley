/*
 * functions.h
 *
 *  Created on: Feb 15, 2020
 *      Author: Adam
 */

#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#include "main.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

uint8_t count;
uint32_t local_time, sensor_time;
uint32_t distance;
uint32_t  data;

void delay (uint32_t us);
uint32_t receive_data (void);
int convert_code (uint32_t code);
uint32_t hcsr04_read (void);
void screenON(int number);
void screenOFF(void);
void display(int cyfra, int czas);
void servo_forward(void);
void servo_backward(void);
void servo_stop(void);

#endif /* INC_FUNCTIONS_H_ */
