/*
 * FireSensor.h
 *
 *  Created on: 5 thg 4, 2026
 *      Author: KHANG
 */

#ifndef LIBRARIES_FIRESENSOR_FIRESENSOR_H_
#define LIBRARIES_FIRESENSOR_FIRESENSOR_H_
#include "main.h"

/* Khởi tạo chân PA6 thành Input thả nổi an toàn */
void FireSensor_Init(void);

/* Hàm kiểm tra trạng thái của cảm biến lửa */
uint8_t FireSensor_IsDetected(void);

#endif /* LIBRARIES_FIRESENSOR_FIRESENSOR_H_ */
