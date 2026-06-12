/*
 * Gas.h
 *
 *  Created on: Jan 24, 2026
 *      Author: LENOVO
 */

#ifndef LIBRARIES_MQ_2_GAS_H_
#define LIBRARIES_MQ_2_GAS_H_
#include "main.h"

/* * Hàm đọc giá trị Analog từ cảm biến khí gas MQ-2.
 * Đầu vào: Con trỏ trỏ đến bộ ADC (ví dụ: &hadc1)
 * Đầu ra: Giá trị ADC từ 0 - 4095 (với độ phân giải 12-bit mặc định của STM32)
 */
uint32_t MQ2_Read(ADC_HandleTypeDef *hadc_handle);




#endif /* LIBRARIES_MQ_2_GAS_H_ */
