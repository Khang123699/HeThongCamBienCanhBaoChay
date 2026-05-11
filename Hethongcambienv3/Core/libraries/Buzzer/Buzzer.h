#ifndef __BUZZER_H
#define __BUZZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"      // Nạp thư viện chứa định nghĩa chân PB5 (BUZZER_Pin)
#include "cmsis_os.h"  // Nạp thư viện RTOS để dùng hàm delay

/* Khởi tạo buzzer (Tắt còi ban đầu để tránh kêu inh ỏi lúc cấp nguồn) */
void BUZZER_Init(void);

/* Bật buzzer (Cấp mức logic HIGH) */
void BUZZER_On(void);

/* Tắt buzzer (Cấp mức logic LOW) */
void BUZZER_Off(void);

/* Đảo trạng thái buzzer (đang kêu -> tắt, đang tắt -> kêu) */
void BUZZER_Toggle(void);

/* Kêu buzzer trong một khoảng thời gian (ms) rồi tự tắt */
void BUZZER_Beep(uint16_t time_ms);

#ifdef __cplusplus
}
#endif

#endif /* __BUZZER_H */
