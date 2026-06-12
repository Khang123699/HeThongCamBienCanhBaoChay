#include "button.h"

#define BUTTON_PORT GPIOB
#define BUTTON_PIN  GPIO_PIN_6
#define BUTTON_DEBOUNCE_MS 30U

static uint8_t  buttonDangNhan = 0;
static uint32_t tickBatDauNhan = 0;
static uint8_t  daBaoGiuDuThoiGian = 0;

/* debounce */
static uint8_t  lastRawState = 0;
static uint8_t  stableState = 0;
static uint32_t tickLastChange = 0;

static uint8_t Button_ReadRaw(void)
{
    /* Pull-up:
       Nhấn = RESET = 1
       Thả  = SET   = 0
    */
    return (HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN) == GPIO_PIN_RESET) ? 1U : 0U;
}

void Button_Init(void)
{
    uint8_t raw = Button_ReadRaw();
    lastRawState = raw;
    stableState = raw;
    tickLastChange = osKernelGetTickCount();
    Button_ResetHoldState();
}

uint8_t Button_IsPressed(void)
{
    uint8_t raw = Button_ReadRaw();
    uint32_t now = osKernelGetTickCount();

    if (raw != lastRawState)
    {
        lastRawState = raw;
        tickLastChange = now;
    }

    if ((now - tickLastChange) >= BUTTON_DEBOUNCE_MS)
    {
        stableState = raw;
    }

    return stableState;
}

uint8_t Button_IsHeld(uint32_t holdTimeMs)
{
    if (Button_IsPressed())
    {
        if (buttonDangNhan == 0U)
        {
            buttonDangNhan = 1U;
            tickBatDauNhan = osKernelGetTickCount();
            daBaoGiuDuThoiGian = 0U;
            return 0U;
        }

        if ((daBaoGiuDuThoiGian == 0U) &&
            ((osKernelGetTickCount() - tickBatDauNhan) >= holdTimeMs))
        {
            daBaoGiuDuThoiGian = 1U;
            return 1U;
        }
    }
    else
    {
        Button_ResetHoldState();
    }

    return 0U;
}

void Button_ResetHoldState(void)
{
    buttonDangNhan = 0U;
    tickBatDauNhan = 0U;
    daBaoGiuDuThoiGian = 0U;
}
