#include "uart.h"
#include "data.h"
#include <Arduino.h>

#define UART_RX2_PIN 26
#define UART_TX2_PIN 27
#define UART_BAUD 115200

static String uartBuffer = "";

static void parseSTM32Line(String line) {
  line.trim();
  if (line.length() == 0) return;

  char stateStr[32] = {0};
  unsigned long mq2Val = 0;
  int tempVal = 0;
  int humVal = 0;
  int fireVal = 0;

  int matched = sscanf(line.c_str(),
    "STATE:%31[^,],MQ2=%lu,TEMP=%d,HUM=%d,FIRE=%d",
    stateStr, &mq2Val, &tempVal, &humVal, &fireVal);

  if (matched == 5) {
    // kiểm tra thay đổi
    if (mq2 != mq2Val || temp != tempVal || hum != humVal ||
        fire != fireVal || trangThai != String(stateStr)) {

      trangThai = String(stateStr);
      mq2 = mq2Val;
      temp = tempVal;
      hum = humVal;
      fire = fireVal;

      dataChanged = true; // báo có dữ liệu mới
    }
  }
}

void UART_Init() {
  Serial2.begin(UART_BAUD, SERIAL_8N1, UART_RX2_PIN, UART_TX2_PIN);
}

void UART_Process() {
  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n') {
      parseSTM32Line(uartBuffer);
      uartBuffer = "";
    } else if (c != '\r') {
      uartBuffer += c;
    }
  }
}