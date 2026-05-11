#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f4xx_hal.h"

/* ================= THÔNG SỐ SERVO ================= */
#define SERVO_MIN_US      500
#define SERVO_MAX_US      2500

/* Định nghĩa lại Channel cho TIM2 (PA0, PA1, PA2) */
#define SERVO1_CH         TIM_CHANNEL_1  // Chân PA0
#define SERVO2_CH         TIM_CHANNEL_2  // Chân PA1
#define SERVO3_CH         TIM_CHANNEL_3  // Chân PA2

/* ================= HÀM API ================= */
void Servo_Init(void);
void Servo_SetAngle(uint8_t servo_id, uint8_t angle);
void Servo_SetPulse(uint8_t servo_id, uint16_t pulse_us);
void Servo_SetAll(uint8_t angle1, uint8_t angle2, uint8_t angle3);

#endif
