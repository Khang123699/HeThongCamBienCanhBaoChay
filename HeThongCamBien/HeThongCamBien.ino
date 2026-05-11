#include <Arduino.h>
#include "uart.h"
#include "web.h"
#include "firebase.h"

void setup()
{
    Serial.begin(115200);

    UART_Init();
    Web_Init();
    FirebaseApp_Init();
}

void loop()
{
    UART_Process();
    Web_HandleClient();
    FirebaseApp_Process();
}