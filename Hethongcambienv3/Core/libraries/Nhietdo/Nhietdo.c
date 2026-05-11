#include "Nhietdo.h"

// ==========================================================
// HÀM HỖ TRỢ: DELAY MICRO-GIÂY BẰNG DWT (Không cần Timer)
// ==========================================================
static void DWT_Delay_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);
    while (DWT->CYCCNT - startTick < delayTicks);
}

// ==========================================================
// HÀM HỖ TRỢ: CHUYỂN ĐỔI CHẾ ĐỘ CHÂN PA5
// ==========================================================
static void Set_Pin_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = NHIETDO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(NHIETDO_PORT, &GPIO_InitStruct);
}

static void Set_Pin_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = NHIETDO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(NHIETDO_PORT, &GPIO_InitStruct);
}

// ==========================================================
// HÀM HỖ TRỢ: ĐỌC 1 BYTE (CÓ BỔ SUNG TIMEOUT CHỐNG TREO)
// ==========================================================
static uint8_t read_byte(uint8_t *err) {
    uint8_t i, j = 0;
    uint32_t timeout; // Biến đếm chống kẹt

    for (i = 0; i < 8; i++) {
        timeout = 0;
        while (!(HAL_GPIO_ReadPin(NHIETDO_PORT, NHIETDO_PIN))) { // Chờ mức cao
            timeout++;
            if (timeout > 50000) { *err = 1; return 0; } // Quá hạn -> Báo lỗi và thoát
        }

        delay_us(40); // Đợi 40us để xem bit là 0 hay 1

        if ((HAL_GPIO_ReadPin(NHIETDO_PORT, NHIETDO_PIN))) {
            j |= (1 << (7 - i)); // Nếu vẫn ở mức cao thì bit này là 1
        }

        timeout = 0;
        while ((HAL_GPIO_ReadPin(NHIETDO_PORT, NHIETDO_PIN))) { // Chờ mức thấp trở lại
            timeout++;
            if (timeout > 50000) { *err = 1; return 0; } // Quá hạn -> Báo lỗi và thoát
        }
    }
    return j;
}

// ==========================================================
// CÁC HÀM CHÍNH GIAO TIẾP VỚI CHƯƠNG TRÌNH
// ==========================================================
void Nhietdo_Init(void) {
    DWT_Delay_Init(); // Bật bộ đếm chu kỳ CPU
    Set_Pin_Output();
    HAL_GPIO_WritePin(NHIETDO_PORT, NHIETDO_PIN, GPIO_PIN_SET); // Kéo mức cao chờ đợi
}

uint8_t Nhietdo_Read(Nhietdo_Data_t *Data) {
    uint8_t rh_byte1 = 0, rh_byte2 = 0, temp_byte1 = 0, temp_byte2 = 0, checksum = 0;
    uint8_t presence = 0;
    uint32_t timeout = 0;
    uint8_t err = 0; // Biến nhận trạng thái lỗi từ hàm read_byte

    // 1. Gửi tín hiệu Start
    Set_Pin_Output();
    HAL_GPIO_WritePin(NHIETDO_PORT, NHIETDO_PIN, GPIO_PIN_RESET);
    HAL_Delay(18); // Kéo xuống ít nhất 18ms
    HAL_GPIO_WritePin(NHIETDO_PORT, NHIETDO_PIN, GPIO_PIN_SET);
    delay_us(20);  // Chờ 20us để DHT11 phản hồi

    // 2. Lắng nghe phản hồi từ DHT11
    Set_Pin_Input();
    delay_us(40);
    if (!(HAL_GPIO_ReadPin(NHIETDO_PORT, NHIETDO_PIN))) {
        delay_us(80);
        if ((HAL_GPIO_ReadPin(NHIETDO_PORT, NHIETDO_PIN))) {
            presence = 1; // Cảm biến đã phản hồi
        }
    }

    // 3. Chờ cảm biến kéo chân xuống mức thấp trở lại (BỔ SUNG TIMEOUT)
    timeout = 0;
    while ((HAL_GPIO_ReadPin(NHIETDO_PORT, NHIETDO_PIN))) {
        timeout++;
        if (timeout > 50000) return 0; // Kẹt thì thoát luôn, trả về 0 (đọc thất bại)
    }

    // Nếu cảm biến có mặt, tiến hành đọc dữ liệu
    if (presence) {
        // 4. Đọc 5 byte dữ liệu và theo dõi biến err
        rh_byte1   = read_byte(&err);
        rh_byte2   = read_byte(&err);
        temp_byte1 = read_byte(&err);
        temp_byte2 = read_byte(&err);
        checksum   = read_byte(&err);

        // Nếu trong quá trình đọc bị FreeRTOS cắt ngang gây timeout -> thoát luôn
        if (err == 1) return 0;

        // 5. Kiểm tra Checksum (Tính toàn vẹn của dữ liệu)
        if ((rh_byte1 + rh_byte2 + temp_byte1 + temp_byte2) == checksum) {
            Data->Humidity = rh_byte1;
            Data->Temperature = temp_byte1;
            return 1; // Đọc thành công
        }
    }

    return 0; // Lỗi mất kết nối hoặc sai checksum
}
