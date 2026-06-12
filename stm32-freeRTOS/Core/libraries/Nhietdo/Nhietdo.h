#ifndef __NHIETDO_H
#define __NHIETDO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// Tự động lấy cấu hình chân cắm từ CubeMX (main.h)
#define NHIETDO_PORT DHT11_GPIO_Port
#define NHIETDO_PIN  DHT11_Pin

// Cấu trúc lưu trữ dữ liệu nhiệt độ, độ ẩm
typedef struct {
    uint8_t Temperature; // Nhiệt độ (độ C)
    uint8_t Humidity;    // Độ ẩm (%)
} Nhietdo_Data_t;

/* Khởi tạo bộ đếm thời gian cực ngắn cho cảm biến */
void Nhietdo_Init(void);

/* * Đọc dữ liệu từ cảm biến DHT11
 * Trả về: 1 nếu thành công, 0 nếu thất bại (lỗi kết nối hoặc sai checksum)
 */
uint8_t Nhietdo_Read(Nhietdo_Data_t *Data);

#ifdef __cplusplus
}
#endif

#endif /* __NHIETDO_H */
