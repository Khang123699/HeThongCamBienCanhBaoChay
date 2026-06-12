#include "Buzzer.h"

/* ==================================================
 * Hàm: BUZZER_Init
 * Mục đích: Tắt buzzer ngay khi khởi động hệ thống
 * ================================================== */
void BUZZER_Init(void)
{
    BUZZER_Off();
}

/* ==================================================
 * Hàm: BUZZER_On
 * Mục đích: Bật buzzer (Chân PB5 lên mức 1)
 * ================================================== */
void BUZZER_On(void)
{
    // Lưu ý: Nếu mạch còi của bạn kích ở mức thấp (Active Low),
    // hãy đổi GPIO_PIN_SET thành GPIO_PIN_RESET nhé.
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}

/* ==================================================
 * Hàm: BUZZER_Off
 * Mục đích: Tắt buzzer (Chân PB5 xuống mức 0)
 * ================================================== */
void BUZZER_Off(void)
{
    // Tương tự, nếu còi kích mức thấp, đổi RESET thành SET.
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
}

/* ==================================================
 * Hàm: BUZZER_Toggle
 * Mục đích: Đảo trạng thái hiện tại của chân còi
 * ================================================== */
void BUZZER_Toggle(void)
{
    HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
}

/* ==================================================
 * Hàm: BUZZER_Beep
 * Mục đích: Bật còi kêu trong time_ms mili-giây rồi tắt
 * Tham số: time_ms - Thời gian kêu (ví dụ: 500 là nửa giây)
 * ================================================== */
void BUZZER_Beep(uint16_t time_ms)
{
    BUZZER_On();
    osDelay(time_ms); // Dùng delay của hệ điều hành FreeRTOS
    BUZZER_Off();
}
