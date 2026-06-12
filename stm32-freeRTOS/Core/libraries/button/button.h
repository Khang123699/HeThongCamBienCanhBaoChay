/*
 * button.h
 *
 *  Created on: Apr 13, 2026
 *      Author: LENOVO
 */

#ifndef LIBRARIES_BUTTON_BUTTON_H_
#define LIBRARIES_BUTTON_BUTTON_H_

#include "main.h"
#include "cmsis_os2.h"
#include <stdint.h>

/* Khởi tạo nút */
void Button_Init(void);

/* Kiểm tra nút đang được nhấn hay không
   Trả về:
   - 1: đang nhấn
   - 0: không nhấn
*/
uint8_t Button_IsPressed(void);

/* Kiểm tra nhấn giữ đủ holdTimeMs chưa
   Trả về:
   - 1: đã giữ đủ thời gian
   - 0: chưa đủ
*/
uint8_t Button_IsHeld(uint32_t holdTimeMs);

/* Reset trạng thái theo dõi giữ nút */
void Button_ResetHoldState(void);


#endif /* LIBRARIES_BUTTON_BUTTON_H_ */
