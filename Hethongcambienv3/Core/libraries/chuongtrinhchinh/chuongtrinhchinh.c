#include "chuongtrinhchinh.h"
#include "cmsis_os2.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#include "../led/led.h"
#include "../Buzzer/Buzzer.h"
#include "../Nhietdo/Nhietdo.h"
#include "../MQ-2/Gas.h"
#include "../FireSensor/FireSensor.h"
#include "../Servo/Servo.h"
#include "../button/button.h"

/* ================= EXTERN HANDLE TỪ main.c ================= */
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;

/* ================= CẤU HÌNH NGƯỠNG ================= */
#define NGUONG_GAS_CANH_BAO        1800U
#define NGUONG_NHIETDO_NGUYHIEM    40U
#define NGUONG_GAS_ANTOAN       700U
#define NGUONG_NHIETDO_ANTOAN    35U
/* Chu kỳ đọc cảm biến */
#define SENSOR_POLL_MS             100U
#define DHT11_READ_PERIOD_MS       2000U

/* Thời gian quay servo để mở/đóng */
#define SERVO_MOVE_TIME_MS         180U

/* Nhấn giữ PB6 đủ 5 giây để thoát mức 3 */
#define BUTTON_HOLD_TIME_MS        5000U

/*thời gian gửi esp32*/
#define UART_SEND_PERIOD_MS        500U

/* ================= KIỂU DỮ LIỆU TRẠNG THÁI ================= */
typedef enum
{
    STATE_NORMAL = 0,
    STATE_LEVEL1_WARNING_GAS,
    STATE_LEVEL2_DANGER_TEMP,
    STATE_LEVEL3_FIRE
} SystemState_t;

/* ================= BIẾN TOÀN CỤC ================= */
Nhietdo_Data_t thongTinNhietDo;
uint32_t giaTriMQ2 = 0;
uint32_t giaTriMQ6 = 0;          
uint8_t phatHienLua = 0;
uint8_t docDHTThanhCong = 0;

/* ===== BIẾN DÙNG CHO DELAY GAS 5s ===== */
static uint32_t tickGasAnToanBatDau = 0;
static uint8_t dangChoGasOnDinh = 0;

/* Trạng thái cửa */
uint8_t cuaSoDaMo = 0;           /* Servo 1 - PA0 */
uint8_t cuaBepDaMo = 0;          /* Servo 2 - PA1 */
uint8_t cuaThoatHiemDaMo = 0;    /* Servo 3 - PA2 */

/* Trạng thái hệ thống */
volatile SystemState_t trangThaiHienTai = STATE_NORMAL;
volatile SystemState_t trangThaiTruocDo = STATE_NORMAL;

/* Khóa mức 3 */
volatile uint8_t khoaCanhBaoChay = 0;

/* ================= TASK HANDLE ================= */
osThreadId_t SensorTaskHandle;
osThreadId_t ControlTaskHandle;
osThreadId_t UartTaskHandle;

const osThreadAttr_t SensorTask_attributes = {
    .name = "SensorTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityNormal
};

const osThreadAttr_t ControlTask_attributes = {
    .name = "ControlTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal
};

const osThreadAttr_t UartTask_attributes = {
    .name = "UartTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityBelowNormal
};

/* =========================================================
   HÀM GỬI UART CHO ESP
========================================================= */
static void ESP_SendString(const char *str)
{
    if (str != NULL)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), 100);
    }
}

static void ESP_SendState(SystemState_t state)
{
    char msg[160];

    switch (state)
    {
        case STATE_NORMAL:
            snprintf(msg, sizeof(msg),
                     "STATE:NORMAL,MQ2=%lu,TEMP=%d,HUM=%d,FIRE=%d\r\n",
                     giaTriMQ2,
                     thongTinNhietDo.Temperature,
                     thongTinNhietDo.Humidity,+
                     phatHienLua);
            break;

        case STATE_LEVEL1_WARNING_GAS:
            snprintf(msg, sizeof(msg),
                     "STATE:LEVEL1_WARNING_GAS,MQ2=%lu,TEMP=%d,HUM=%d,FIRE=%d\r\n",
                     giaTriMQ2,
                     thongTinNhietDo.Temperature,
                     thongTinNhietDo.Humidity,
                     phatHienLua);
            break;

        case STATE_LEVEL2_DANGER_TEMP:
            snprintf(msg, sizeof(msg),
                     "STATE:LEVEL2_DANGER_TEMP,MQ2=%lu,TEMP=%d,HUM=%d,FIRE=%d\r\n",
                     giaTriMQ2,
                     thongTinNhietDo.Temperature,
                     thongTinNhietDo.Humidity,
                     phatHienLua);
            break;

        case STATE_LEVEL3_FIRE:
            snprintf(msg, sizeof(msg),
                     "STATE:LEVEL3_FIRE,MQ2=%lu,TEMP=%d,HUM=%d,FIRE=%d\r\n",
                     giaTriMQ2,
                     thongTinNhietDo.Temperature,
                     thongTinNhietDo.Humidity,
                     phatHienLua);
            break;

        default:
            return;
    }

    ESP_SendString(msg);
}

static void ESP_SendLevel3ReleasedEvent(void)
{
    ESP_SendString("EVENT:LEVEL3_UNLOCK_BY_BUTTON_5S\r\n");
}

/* =========================================================
   HÀM ĐIỀU KHIỂN SERVO
========================================================= */
static void MoCua(uint8_t servo_id, uint8_t *trangThaiCua)
{
    if (*trangThaiCua == 0)
    {
        Servo_SetAngle(servo_id, 0);
        osDelay(SERVO_MOVE_TIME_MS);
        Servo_SetAngle(servo_id, 90);
        *trangThaiCua = 1;
    }
}

static void DongCua(uint8_t servo_id, uint8_t *trangThaiCua)
{
    if (*trangThaiCua == 1)
    {
        Servo_SetAngle(servo_id, 180);
        osDelay(SERVO_MOVE_TIME_MS);
        Servo_SetAngle(servo_id, 90);
        *trangThaiCua = 0;
    }
}

static void MoCuaSo(void)          { MoCua(1, &cuaSoDaMo); }
static void DongCuaSo(void)        { DongCua(1, &cuaSoDaMo); }

static void MoCuaBep(void)         { MoCua(2, &cuaBepDaMo); }
static void DongCuaBep(void)       { DongCua(2, &cuaBepDaMo); }

static void MoCuaThoatHiem(void)   { MoCua(3, &cuaThoatHiemDaMo); }
static void DongCuaThoatHiem(void) { DongCua(3, &cuaThoatHiemDaMo); }

static void MoTatCaCua(void)
{
    MoCuaSo();
    MoCuaBep();
    MoCuaThoatHiem();
}

static void DongTatCaCua(void)
{
    DongCuaSo();
    DongCuaBep();
    DongCuaThoatHiem();
}

/* =========================================================
   XÁC ĐỊNH MỨC CẢNH BÁO
   Ưu tiên:
   - Nếu đã khóa mức 3 -> luôn giữ mức 3
   - Nếu phát hiện lửa -> vào mức 3 và khóa lại
   - Nếu không thì xét mức 2 -> mức 1 -> NORMAL
========================================================= */
static SystemState_t XacDinhTrangThai(void)
{
    /* Nếu đã khóa mức 3 */
    if (khoaCanhBaoChay == 1U)
    {
        return STATE_LEVEL3_FIRE;
    }

    /* Nếu phát hiện lửa */
    if (phatHienLua == 1U)
    {
        khoaCanhBaoChay = 1U;
        return STATE_LEVEL3_FIRE;
    }

    /* ================= LEVEL 2 - NHIỆT ĐỘ ================= */
    if (docDHTThanhCong == 1U)
    {
        /* Nếu đang ở level 2 rồi → chỉ thoát khi xuống dưới ngưỡng an toàn */
        if (trangThaiTruocDo == STATE_LEVEL2_DANGER_TEMP)
        {
            if (thongTinNhietDo.Temperature > NGUONG_NHIETDO_ANTOAN)
            {
                return STATE_LEVEL2_DANGER_TEMP;
            }
        }
        else
        {
            /* Nếu chưa ở level 2 → xét vào mức nguy hiểm */
            if (thongTinNhietDo.Temperature >= NGUONG_NHIETDO_NGUYHIEM)
            {
                return STATE_LEVEL2_DANGER_TEMP;
            }
        }
    }

    /* ================= LEVEL 1 - GAS ================= */
    if (trangThaiTruocDo == STATE_LEVEL1_WARNING_GAS)
    {
        if (giaTriMQ2 > NGUONG_GAS_ANTOAN)
        {
            /* Vẫn còn nguy hiểm -> reset timer */
            dangChoGasOnDinh = 0;
            return STATE_LEVEL1_WARNING_GAS;
        }
        else
        {
            /* Đã xuống mức an toàn -> bắt đầu đếm 5s */
            if (dangChoGasOnDinh == 0)
            {
                tickGasAnToanBatDau = osKernelGetTickCount();
                dangChoGasOnDinh = 1;
            }

            /* Nếu chưa đủ 5s -> vẫn giữ trạng thái LEVEL 1 */
            if ((osKernelGetTickCount() - tickGasAnToanBatDau) < 5000)
            {
                return STATE_LEVEL1_WARNING_GAS;
            }

            /* Đủ 5s -> cho phép về NORMAL */
            dangChoGasOnDinh = 0;
        }
    }
    else
    {
        /* Chưa cảnh báo -> xét ngưỡng nguy hiểm */
        if (giaTriMQ2 >= NGUONG_GAS_CANH_BAO)
        {
            return STATE_LEVEL1_WARNING_GAS;
        }
    }

    return STATE_NORMAL;
}
/* =========================================================
   CHỈ CHO PHÉP THOÁT KHỎI MỨC 3 KHI GIỮ NÚT 5 GIÂY
========================================================= */
static void KiemTraNutDeMoKhoaMuc3(void)
{
    if (trangThaiHienTai != STATE_LEVEL3_FIRE)
    {
        Button_ResetHoldState();
        return;
    }

    if (Button_IsHeld(BUTTON_HOLD_TIME_MS))
    {
        khoaCanhBaoChay = 0U;
        ESP_SendLevel3ReleasedEvent();
    }
}

/* =========================================================
   HÀNH ĐỘNG KHI MỚI CHUYỂN SANG 1 TRẠNG THÁI
========================================================= */
static void XuLyKhiVaoTrangThaiMoi(SystemState_t state)
{
    switch (state)
    {
        case STATE_NORMAL:
            BUZZER_Off();
            LED_Red_Off();
            LED_Green_Off();
            DongTatCaCua();
            ESP_SendState(state);
            break;

        case STATE_LEVEL1_WARNING_GAS:
            BUZZER_Off();
            LED_Green_Off();
            MoCuaSo();
            ESP_SendState(state);
            break;

        case STATE_LEVEL2_DANGER_TEMP:
            LED_Green_Off();
            MoCuaSo();
            MoCuaBep();
            ESP_SendState(state);
            break;

        case STATE_LEVEL3_FIRE:
        	BUZZER_On();
            MoTatCaCua();
            LED_Green_On();
            ESP_SendState(state);
            break;

        default:
            break;
    }
}

/* =========================================================
   HÀNH ĐỘNG DUY TRÌ THEO TỪNG MỨC
========================================================= */
static void XuLyTrangThaiLienTuc(SystemState_t state)
{
    switch (state)
    {
        case STATE_NORMAL:
            BUZZER_Off();
            LED_Red_Off();
            LED_Green_Off();
            osDelay(100);
            break;

        case STATE_LEVEL1_WARNING_GAS:
            /* Mức 1: tự tắt khi không còn vượt ngưỡng */
            BUZZER_Off();
            LED_Green_Off();

            LED_Red_On();
            osDelay(500);
            LED_Red_Off();
            osDelay(500);
            break;

        case STATE_LEVEL2_DANGER_TEMP:
            /* Mức 2: tự tắt khi không còn vượt ngưỡng */
            LED_Green_Off();

            LED_Red_On();
            BUZZER_On();
            osDelay(300);

            LED_Red_Off();
            BUZZER_Off();
            osDelay(700);
            break;

        case STATE_LEVEL3_FIRE:
            /* Mức 3: đã khóa thì không tự tắt, chỉ nút nhấn mới mở khóa */
            LED_Green_On();
            BUZZER_On();

            LED_Red_On();
            osDelay(100);
            LED_Red_Off();
            osDelay(100);
            break;

        default:
            osDelay(100);
            break;
    }
}

/* =========================================================
   TASK ĐỌC CẢM BIẾN
========================================================= */
void StartSensorTask(void *argument)
{
    uint32_t tickDocDHT = 0U;

    for (;;)
    {
        /* 1. Đọc MQ2 tại PA3 */
        giaTriMQ2 = MQ2_Read(&hadc1);

        /* 2. Đọc cảm biến lửa tại PA6 */
        phatHienLua = FireSensor_IsDetected();

        /* 3. Đọc DHT11 tại PA5 mỗi 2 giây */
        if ((osKernelGetTickCount() - tickDocDHT) >= DHT11_READ_PERIOD_MS)
        {
            if (Nhietdo_Read(&thongTinNhietDo) == 1U)
            {
                docDHTThanhCong = 1U;
            }
            else
            {
                docDHTThanhCong = 0U;
            }

            tickDocDHT = osKernelGetTickCount();
        }

        osDelay(SENSOR_POLL_MS);
    }
}

/* =========================================================
   TASK ĐIỀU KHIỂN CHÍNH
========================================================= */
void StartControlTask(void *argument)
{
    Servo_SetAngle(1, 90);
    Servo_SetAngle(2, 90);
    Servo_SetAngle(3, 90);

    BUZZER_Off();
    LED_Red_Off();
    LED_Green_Off();

    ESP_SendString("SYSTEM:BOOT\r\n");

    trangThaiHienTai = STATE_NORMAL;
    trangThaiTruocDo = (SystemState_t)255U;

    for (;;)
    {
        /* Nếu đang ở mức 3 thì kiểm tra nút để mở khóa */
        KiemTraNutDeMoKhoaMuc3();

        /* Sau đó mới đánh giá lại trạng thái */
        trangThaiHienTai = XacDinhTrangThai();

        if (trangThaiHienTai != trangThaiTruocDo)
        {
            XuLyKhiVaoTrangThaiMoi(trangThaiHienTai);
            trangThaiTruocDo = trangThaiHienTai;
        }

        XuLyTrangThaiLienTuc(trangThaiHienTai);
    }
}

void StartUartTask(void *argument)
{
    for (;;)
    {
        ESP_SendState(trangThaiHienTai);
        osDelay(UART_SEND_PERIOD_MS);
    }
}

/* =========================================================
   HÀM KHỞI TẠO TOÀN HỆ THỐNG
========================================================= */
void KhoiTao(void)
{
    LED_Init();
    BUZZER_Init();
    Servo_Init();
    Nhietdo_Init();
    FireSensor_Init();
    Button_Init();

    Servo_SetAngle(1, 90);
    Servo_SetAngle(2, 90);
    Servo_SetAngle(3, 90);

    SensorTaskHandle  = osThreadNew(StartSensorTask, NULL, &SensorTask_attributes);
    ControlTaskHandle = osThreadNew(StartControlTask, NULL, &ControlTask_attributes);
    UartTaskHandle    = osThreadNew(StartUartTask, NULL, &UartTask_attributes);
}
