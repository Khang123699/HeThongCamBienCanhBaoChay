/*
 * led.h
 *
 * Created on: Jan 22, 2026
 * Author: LENOVO
 */

#ifndef LIBRARIES_LED_LED_H_
#define LIBRARIES_LED_LED_H_

#include "stm32f4xx_hal.h"
#include "main.h"

// Hàm khởi tạo ban đầu (Tắt tất cả LED)
void LED_Init(void);

// ==========================================
// CÁC HÀM ĐIỀU KHIỂN LED ĐỎ (Chân PB4)
// ==========================================
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Red_Toggle(void);

// ==========================================
// CÁC HÀM ĐIỀU KHIỂN LED XANH (Chân PB3)
// ==========================================
void LED_Green_On(void);
void LED_Green_Off(void);
void LED_Green_Toggle(void);



#endif /* LIBRARIES_LED_LED_H_ */
