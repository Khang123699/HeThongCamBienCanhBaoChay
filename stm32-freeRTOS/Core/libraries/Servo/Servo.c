#include "Servo.h"

/* Timer handle từ CubeMX */
extern TIM_HandleTypeDef htim2;// Dùng cho PA1 (Servo 2) và PA2 (Servo 3)
extern TIM_HandleTypeDef htim3;
//extern TIM_HandleTypeDef htim2; // Dùng cho PB0 (Servo 1)

void Servo_Init(void)
{
    /* Kích hoạt PWM cho 3 kênh trên 2 Timer khác nhau */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // Kích hoạt PB0 (Servo 1)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Kích hoạt PA1 (Servo 2)
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // Kích hoạt PA2 (Servo 3)

    /* Đã tách ra: Khóa từng Servo ở trạng thái đứng im (90 độ) */
    Servo_SetAngle(1, 90);
    Servo_SetAngle(2, 90);
    Servo_SetAngle(3, 90);
}

void Servo_SetPulse(uint8_t servo_id, uint16_t pulse_us)
{
    if (pulse_us < SERVO_MIN_US) pulse_us = SERVO_MIN_US;
    if (pulse_us > SERVO_MAX_US) pulse_us = SERVO_MAX_US;

    /* Phân luồng Timer tùy theo Servo nào đang được gọi */
    if (servo_id == 1) {
        // Servo 1 dùng PB0 (Timer 3, Kênh 3)
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pulse_us);
    }
    else if (servo_id == 2) {
        // Servo 2 dùng PA1 (Timer 2, Kênh 2)
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse_us);
    }
    else if (servo_id == 3) {
        // Servo 3 dùng PA2 (Timer 2, Kênh 3)
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_us);
    }
}

void Servo_SetAngle(uint8_t servo_id, uint8_t angle)
{
    if (angle > 180) angle = 180;

    /* Công thức quy đổi góc 0-180 sang độ rộng xung 500-2500us */
    uint16_t pulse = SERVO_MIN_US + ((uint32_t)angle * (SERVO_MAX_US - SERVO_MIN_US)) / 180;

    Servo_SetPulse(servo_id, pulse);
}

