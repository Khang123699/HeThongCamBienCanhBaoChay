# SMART FIRE WARNING SENSOR SYSTEM (IoT)
> **An integrated smart fire warning system featuring STM32F401 (FreeRTOS RTOS), ESP32 Gateway (Firebase & Web Dashboard), and RC Servo motors for automated emergency exit door control.**

---

## WARNING LEVELS

The system dynamically shifts warning levels based on sensor inputs:

| State | Threshold | Buzzer | LED Indicators | Servo Door States | Special Behavior |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Normal** *(Level 0)* | Safe | OFF | Red: OFF, Green: OFF | All doors closed | Continuous monitoring. |
| **Gas Warning** *(Level 1)* | MQ-2 Gas $\ge$ 1800 | OFF | Red: blinks slowly (0.5s) | **Open Window** | Automatically resets to Level 0 after gas drops below 700 for 5s. |
| **Temp Danger** *(Level 2)* | Temp $\ge$ 40°C | Intermittent (0.3s) | Red: blinks intermittently | **Open Window & Kitchen Door** | Automatically resets to Level 0 when temp falls below 35°C. |
| **FIRE ALARM** *(Level 3)* | Flame Detected | Continuous | Green: ON, Red: blinks fast | **Open all 3 doors** | **Latch mechanism**. Only resets after holding button `PB6` for 5s. |

> [!IMPORTANT]
> **Level 3 Latch Mechanism**: When a flame is detected, the system locks into Level 3. Even if the flame is extinguished, the alarm remains active and doors stay open. To reset the system to Normal, a supervisor must inspect the scene and hold down the STM32 button (`PB6`) for **5 seconds**.

---

## HARDWARE & PIN MAPPING

### 1. STM32F401CCU6 (Blackpill)
*   **MQ-2 Gas Sensor**: `PA3` (ADC1_IN3)
*   **DHT11 Temp/Humidity**: `PA5` (GPIO 1-Wire)
*   **Flame Sensor**: `PA7` (GPIO Input Pull-up, Active Low)
*   **Servo 1 (Window)**: `PB0` (TIM3_CH3 PWM)
*   **Servo 2 (Kitchen Door)**: `PA1` (TIM2_CH2 PWM)
*   **Servo 3 (Emergency Exit)**: `PA2` (TIM2_CH3 PWM)
*   **Buzzer**: `PB5` (GPIO Output)
*   **Red LED (Warning)**: `PB4` | **Green LED (Exit)**: `PB3`
*   **Level 3 Reset Button**: `PB6` (GPIO Input Pull-up)
*   **UART1 (ESP32 Comm)**: `PA9` (TX) & `PA10` (RX)

### 2. ESP32 Gateway
*   `Pin 26 (RX2)` $\rightarrow$ STM32 `PA9 (TX)`
*   `Pin 27 (TX2)` $\rightarrow$ STM32 `PA10 (RX)`
*   `GND` $\rightarrow$ Common Ground with STM32

---

## DIRECTORY STRUCTURE

```text
HeThongCanhBaoChay/
├── ESP/                 # ESP32 Firmware (Arduino IDE)
│   ├── ESP.ino          # Main program entry
│   ├── web.cpp          # Local WiFi AP/STA & Web Dashboard
│   └── firebase.cpp     # Firebase integration via HTTP REST API
├── stm32-freeRTOS/      # STM32F401 Firmware (STM32CubeIDE)
│   ├── Core/Src/main.c  # Main system initialization
│   ├── Core/Src/freertos.c # FreeRTOS control tasks
│   └── Core/libraries/  # Peripherals drivers (MQ-2, DHT11, Servo, Buzzer, LED, Button)
└── README.md            # Project documentation
```

---

## QUICK START

### 1. Flash STM32 Firmware
1. Open the `stm32-freeRTOS` directory in **STM32CubeIDE**.
2. Compile/Build the project.
3. Connect the ST-Link V2 programmer and flash the code onto the STM32 board.

### 2. Flash ESP32 Firmware
1. Open `ESP.ino` in the `ESP/` directory using **Arduino IDE**.
2. Configure your Firebase Realtime Database URL in `firebase.cpp`:
   ```cpp
   #define DATABASE_URL "https://your-project-default-rtdb.firebaseio.com"
   ```
3. Choose the `ESP32 Dev Module` board, select the COM port, and click **Upload**.

### 3. Execution
*   On power-up, ESP32 broadcasts a WiFi Access Point: **SSID**: `Nhom1_To1` (Password: `12345678`).
*   Connect your device, navigate to `192.168.4.1` to set up home WiFi and view the Local Dashboard.
*   The ESP32 will automatically sync sensor data to Firebase and update the remote/local dashboards.
