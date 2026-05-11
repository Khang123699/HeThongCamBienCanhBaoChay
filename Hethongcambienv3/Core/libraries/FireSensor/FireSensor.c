/*
 * FireSensor.c
 *
 *  Created on: 5 thg 4, 2026
 *      Author: KHANG
 */


#include "FireSensor.h"

#define FIRE_SENSOR_PORT GPIOA
#define FIRE_SENSOR_PIN  GPIO_PIN_7

void FireSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = FIRE_SENSOR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(FIRE_SENSOR_PORT, &GPIO_InitStruct);
}

uint8_t FireSensor_IsDetected(void) {
    // Đọc trạng thái chân PA7.
    // Nếu cảm biến trả về mức LOW (GPIO_PIN_RESET) -> Có lửa
    if (HAL_GPIO_ReadPin(FIRE_SENSOR_PORT, FIRE_SENSOR_PIN) == GPIO_PIN_RESET) {
        return 1; // Phát hiện lửa
    } else {
        return 0; // Không phát hiện lửa
    }
}
