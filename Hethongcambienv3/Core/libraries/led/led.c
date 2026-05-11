#include "led.h"

// Hàm khởi tạo: Tắt tất cả LED và Còi khi mới cấp điện
void LED_Init(void) {
    LED_Red_Off();
    LED_Green_Off();

}

/* ---------------- ĐIỀU KHIỂN LED ĐỎ (PB4) ---------------- */
void LED_Red_On(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
}

void LED_Red_Off(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}

void LED_Red_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
}

/* ---------------- ĐIỀU KHIỂN LED XANH (PB3) ---------------- */
void LED_Green_On(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}

void LED_Green_Off(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
}

void LED_Green_Toggle(void) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
}


