

#include "Gas.h"
#include "FreeRTOS.h"
#include "task.h"

uint32_t MQ2_Read(ADC_HandleTypeDef *hadc_handle) {
    uint32_t adc_value = 0;

    // Bắt đầu quá trình chuyển đổi ADC
    HAL_ADC_Start(hadc_handle);

    // Đợi chuyển đổi hoàn tất với thời gian chờ (timeout) tối đa 10ms
    // Việc này đảm bảo hệ thống không bị treo cứng nếu ADC gặp lỗi
    if (HAL_ADC_PollForConversion(hadc_handle, 10) == HAL_OK) {
        // Lấy giá trị sau khi chuyển đổi thành công
        adc_value = HAL_ADC_GetValue(hadc_handle);
    }

    // Dừng bộ ADC lại
    HAL_ADC_Stop(hadc_handle);

    // KẾT HỢP FREERTOS:
    // Tạm dừng Task hiện tại 1 khoảng thời gian nhỏ (1 tick)
    // Điều này nhường quyền thực thi cho các Task khác, tránh việc hàm đọc
    // độc chiếm CPU nếu được gọi liên tục trong vòng lặp while(1) của một Task.
    vTaskDelay(1);

    return adc_value;
}
