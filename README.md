# 🚨 HỆ THỐNG CẢM BIẾN CẢNH BÁO CHÁY THÔNG MINH (IoT)
> **Dự án tích hợp Vi điều khiển STM32F401, Hệ điều hành thời gian thực (FreeRTOS) và ESP32 Gateway kết nối Firebase & Web Dashboard.**

[![STM32](https://img.shields.io/badge/MCU-STM32F401CCU6-blue.svg?style=for-the-badge&logo=stmicroelectronics)](https://www.st.com/en/microcontrollers-microprocessors/stm32f401cc.html)
[![FreeRTOS](https://img.shields.io/badge/OS-FreeRTOS-green.svg?style=for-the-badge&logo=google-cloud)](https://www.freertos.org/)
[![ESP32](https://img.shields.io/badge/IoT-ESP32--WROOM--32-orange.svg?style=for-the-badge&logo=espressif)](https://www.espressif.com/en/products/socs/esp32)
[![Firebase](https://img.shields.io/badge/Database-Firebase_RTDB-ffca28.svg?style=for-the-badge&logo=firebase)](https://firebase.google.com/)
[![License](https://img.shields.io/badge/License-MIT-brightgreen.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)

---

## 📌 Mục lục
1. [Giới thiệu dự án](#-giới-thiệu-dự-án)
2. [Các công nghệ sử dụng](#-các-công-nghệ-sử-dụng)
3. [Sơ đồ kiến trúc & Nguyên lý hoạt động](#-sơ-đồ-kiến-trúc--nguyên-lý-hoạt-động)
4. [Sơ đồ kết nối chân (Pin Mapping)](#-sơ-đồ-kết-nối-chân-pin-mapping)
5. [Các mức cảnh báo của hệ thống](#-các-mức-cảnh-báo-của-hệ-thống)
6. [Cấu trúc thư mục nguồn](#-cấu-trúc-thư-mục-nguồn)
7. [Hướng dẫn cài đặt & Triển khai](#-hướng-dẫn-cài-đặt--triển-khai)
8. [Giao diện Web Dashboard](#-giao-di diện-web-dashboard)
9. [Thành viên thực hiện](#-thành-viên-thực-hiện)

---

## 📖 Giới thiệu dự án

Hệ thống **Cảnh báo cháy thông minh** là một giải pháp giám sát an toàn nhà ở và nhà xưởng toàn diện. Dự án sử dụng hệ thống hai vi điều khiển (Dual-MCU) giao tiếp thông qua giao thức UART:
*   **STM32F401 (Chạy FreeRTOS)** đóng vai trò là "Bộ não điều khiển trung tâm" (Core Controller). Nó thực hiện việc thu thập dữ liệu cảm biến thời gian thực, quản lý máy trạng thái (State Machine) cảnh báo, kích hoạt còi báo, hệ thống LED chỉ thị và điều khiển các động cơ Servo tự động mở cửa thoát hiểm/cửa sổ khi có sự cố.
*   **ESP32 NodeMCU** đóng vai trò là "Cổng kết nối IoT" (Communication Gateway). Nó nhận dữ liệu phân tích từ STM32, phát WiFi cấu hình (Captive Web Portal), hiển thị trang giám sát nội bộ (Local Dashboard) và đồng bộ dữ liệu liên tục lên đám mây Firebase Realtime Database để người dùng theo dõi từ xa.

---

## 🛠 Các công nghệ sử dụng

*   **Firmware STM32**:
    *   **Ngôn ngữ**: C (Chuẩn C99)
    *   **HĐH thời gian thực**: FreeRTOS (CMSIS_OS2 API) để quản lý đa tác vụ đa nhiệm.
    *   **Công cụ**: STM32CubeMX & STM32CubeIDE.
*   **Firmware ESP32 (Gateway)**:
    *   **Ngôn ngữ**: C++ (Arduino Framework).
    *   **Tính năng Web**: `WebServer`, `WiFi` (AP & STA Modes) dùng cấu hình mạng linh hoạt.
    *   **Đám mây**: Firebase REST API (sử dụng PATCH/POST gọn nhẹ, không dùng thư viện nặng).
*   **Giao diện Dashboard**: HTML5, CSS3 (Giao diện Dark Mode hiện đại, phản hồi nhanh), Vanilla JavaScript.

---

## 📐 Sơ đồ kiến trúc & Nguyên lý hoạt động

Hệ thống hoạt động dựa trên mô hình xử lý phân tán:

```mermaid
graph TD
    subgraph STM32F401 ["STM32F401 (FreeRTOS Central)"]
        Sensors["Cảm biến MQ-2, DHT11, Cảm biến Lửa"] -->|Đọc liên tục| SensorTask["SensorTask (100ms)"]
        SensorTask -->|Cập nhật biến toàn cục| ControlTask["ControlTask (Main Machine State)"]
        ControlTask -->|Kích hoạt| Actuators["Báo động (Buzzer, LEDs) & Servo Cửa"]
        ControlTask -->|Nhấn giữ 5s để Reset Mức 3| Button["Nút nhấn PB6"]
        ControlTask -->|Đọc trạng thái| UartTask["UartTask (500ms)"]
    end

    subgraph Communication ["Giao tiếp UART (115200)"]
        UartTask -->|Gửi chuỗi dữ liệu định dạng| UART_Line["STATE:<state>,MQ2=<val>,TEMP=<val>,HUM=<val>,FIRE=<val>"]
    end

    subgraph ESP32 ["ESP32 IoT Gateway"]
        UART_Line -->|Nhận & Parse sscanf| ESP32_Core["ESP32 Core Program"]
        ESP32_Core -->|HTTP PATCH /sensor.json| Firebase["Firebase RTDB"]
        ESP32_Core -->|HTTP POST /history.json (nếu Cháy)| FirebaseHist["History Logs"]
        ESP32_Core -->|Cung cấp API /data| WebServer["Local Web Dashboard (192.168.4.1)"]
    end
    
    Firebase -->|Đồng bộ thời gian thực| User["Ứng dụng di động / Web client từ xa"]
    WebServer -->|Truy cập qua trình duyệt| Browser["Điện thoại / Laptop nội bộ"]
```

---

## 🔌 Sơ đồ kết nối chân (Pin Mapping)

### 1. Vi điều khiển STM32F401CCU6 (Blackpill)

| Ngoại vi / Thiết bị | Chân STM32 | Chế độ cấu hình | Mô tả chi tiết |
| :--- | :---: | :---: | :--- |
| **Cảm biến khói/gas MQ-2** | `PA3` | ADC1_IN3 | Đọc giá trị Analog của nồng độ khói/gas |
| **Cảm biến nhiệt/ẩm DHT11** | `PA5` | GPIO Input/Output | Đọc dữ liệu số theo giao thức 1-wire |
| **Cảm biến lửa (Flame)** | `PA7` | GPIO Input (Pull-up) | Mức `LOW` khi phát hiện tia lửa hồng ngoại |
| **Động cơ Servo 1** | `PB0` | TIM3_CH3 (PWM) | Kích hoạt mở/đóng **Cửa sổ** |
| **Động cơ Servo 2** | `PA1` | TIM2_CH2 (PWM) | Kích hoạt mở/đóng **Cửa bếp** |
| **Động cơ Servo 3** | `PA2` | TIM2_CH3 (PWM) | Kích hoạt mở/đóng **Cửa thoát hiểm** |
| **Còi báo động (Buzzer)** | `PB5` | GPIO Output | Còi chip cảnh báo âm thanh |
| **LED Đỏ (Cảnh báo)** | `PB4` | GPIO Output | LED nhấp nháy báo sự cố nguy hiểm |
| **LED Xanh (Lối thoát)** | `PB3` | GPIO Output | LED chỉ thị lối thoát hiểm sáng khi cháy |
| **Nút nhấn Reset Mức 3** | `PB6` | GPIO Input (Pull-up) | Nhấn giữ đủ 5 giây để tắt chuông báo cháy |
| **UART1 TX (Truyền đi)** | `PA9` | USART1_TX | Kết nối sang chân RX của ESP32 |
| **UART1 RX (Nhận về)** | `PA10` | USART1_RX | Kết nối sang chân TX của ESP32 (tùy chọn) |

### 2. Mô đun ESP32 Gateway

| Chân ESP32 | Kết nối sang | Vai trò / Nhiệm vụ |
| :---: | :---: | :--- |
| `Pin 26 (RX2)`| Chân `PA9 (TX)` của STM32 | Nhận luồng dữ liệu trạng thái từ STM32 |
| `Pin 27 (TX2)`| Chân `PA10 (RX)` của STM32| Phản hồi sự kiện về STM32 (khi cần) |
| `GND` | Chân `GND` của STM32 | Nối đất chung cho hai vi điều khiển |

---

## 🚨 Các mức cảnh báo của hệ thống

Hệ thống tự động chuyển đổi hành vi dựa trên kết quả đọc từ các cảm biến:

| Trạng thái (State) | Ngưỡng kích hoạt | Còi báo (Buzzer) | LED chỉ thị | Trạng thái các cửa (Servos) | Hành vi đặc biệt |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **STATE_NORMAL** *(Mức 0)* | Bình thường | Tắt hoàn toàn | Đỏ tắt, Xanh tắt | Đóng tất cả các cửa | Hệ thống ở trạng thái giám sát an toàn. |
| **STATE_LEVEL1_WARNING_GAS** *(Mức 1)* | Gas $\ge$ 1800 | Tắt | Đỏ nháy chậm (0.5s) | **Mở cửa sổ** | Mở cửa sổ thoát khí. Trở về Mức 0 sau 5s ổn định (< 700). |
| **STATE_LEVEL2_DANGER_TEMP** *(Mức 2)* | Nhiệt độ $\ge$ 40°C | Kêu ngắt quãng (0.3s) | Đỏ nháy ngắt quãng | **Mở cửa sổ & Cửa bếp** | Cảnh báo quá nhiệt nguy hiểm. Tự động thoát khi hạ nhiệt (< 35°C). |
| **STATE_LEVEL3_FIRE** *(Mức 3)* | Phát hiện lửa | Kêu liên tục dồn dập | Xanh sáng, Đỏ nháy nhanh | **Mở toàn bộ 3 cửa** | **Khóa trạng thái báo động**. Chỉ giải phóng khi nhấn giữ nút PB6 trong 5 giây. |

> [!IMPORTANT]
> **Cơ chế khóa mức 3**: Khi cảm biến lửa phát hiện có đám cháy, hệ thống lập tức khóa chặt ở **Mức 3**. Ngay cả khi đám cháy đã được dập tắt (cảm biến không còn nhận tín hiệu lửa), chuông báo và lối thoát hiểm vẫn mở để đảm bảo an toàn tuyệt đối. Người giám sát phải kiểm tra hiện trường trực tiếp và nhấn giữ nút bấm trên STM32 (chân `PB6`) trong **5 giây** để đưa hệ thống về trạng thái bình thường.

---

## 📁 Cấu trúc thư mục nguồn

```text
HeThongCanhBaoChay/
├── ESP/                         # Firmware cho ESP32 (Arduino IDE)
│   ├── ESP.ino                  # Điểm khởi chạy Setup & Loop chính
│   ├── data.h / data.cpp        # Định nghĩa các biến cảm biến dùng chung
│   ├── uart.h / uart.cpp        # Quản lý bộ đệm & Parse gói tin UART từ STM32
│   ├── web.h / web.cpp          # Quản lý WiFi AP/STA, HTML portal & Local Dashboard
│   └── firebase.h / firebase.cpp# Giao tiếp Firebase qua HTTP REST API & đồng bộ NTP
│
├── Hethongcambienv3/            # Firmware cho STM32F401 (STM32CubeIDE)
│   ├── Core/
│   │   ├── Src/
│   │   │   ├── main.c           # File sinh mặc định của CubeMX, gọi hàm KhoiTao()
│   │   │   └── freertos.c       # Định nghĩa các luồng nhiệm vụ hệ điều hành
│   │   ├── Inc/
│   │   │   └── main.h           # Định nghĩa cấu hình phần cứng chân GPIO
│   │   └── libraries/           # Thư viện driver tự viết cho từng ngoại vi
│   │       ├── chuongtrinhchinh/# State Machine & Logic xử lý chính (3 Task FreeRTOS)
│   │       ├── MQ-2/            # Đọc kênh ADC từ cảm biến gas
│   │       ├── Nhietdo/         # Đọc và giải mã dữ liệu DHT11
│   │       ├── FireSensor/      # Đọc trạng thái cảm biến lửa
│   │       ├── Servo/           # Điều khiển góc quay Servo qua PWM
│   │       ├── Buzzer/          # Điều khiển tần số còi báo
│   │       ├── led/             # Bật/Tắt/Nháy các LED chỉ thị
│   │       └── button/          # Kiểm tra điều kiện nhấn và giữ nút nhấn
│   └── Hethongcambienv2.ioc     # File cấu hình chân phần cứng của STM32CubeMX
└── README.md                    # Hướng dẫn chi tiết dự án (Tài liệu hiện tại)
```

---

## 🚀 Hướng dẫn cài đặt & Triển khai

### 1. Chuẩn bị phần cứng
*   1x Board STM32F401CCU6 (Blackpill) & Mạch nạp ST-Link V2.
*   1x Board ESP32 NodeMCU.
*   3x Động cơ RC Servo (SG90 hoặc MG90S).
*   1x Cảm biến nhiệt độ DHT11.
*   1x Cảm biến Gas MQ-2.
*   1x Cảm biến phát hiện lửa (Active Low).
*   1x Còi Buzzer 5V, 2x LED (Đỏ, Xanh), 1x Nút nhấn hành trình.
*   Nguồn cấp 5V-2A ổn định (Do hệ thống chạy 3 động cơ Servo đồng thời).

### 2. Triển khai Firmware cho STM32
1.  Tải và cài đặt phần mềm [STM32CubeIDE].
2.  Mở thư mục `Hethongcambienv3` bằng STM32CubeIDE.
3.  Biên dịch dự án.
4.  Kết nối ST-Link V2 từ máy tính sang mạch STM32 và tiến hành nạp chương trình (`Run` / `Debug`).

### 3. Triển khai Firmware cho ESP32
1.  Mở phần mềm Arduino IDE (Yêu cầu đã cài đặt gói thư viện ESP32 Core).
2.  Mở file `ESP.ino` nằm trong thư mục `ESP`.
3.  Cập nhật địa chỉ Firebase Realtime Database của bạn tại file `firebase.cpp` (nếu cần thiết):
    ```cpp
    #define DATABASE_URL "https://ten-du-an-default-rtdb.firebaseio.com"
    ```
4.  Chọn board `ESP32 Dev Module` và cổng COM tương ứng.
5.  Nhấn nạp code (`Upload`).

---

## 🖥 Giao diện Web Dashboard

Khi cấp nguồn, ESP32 sẽ phát ra một điểm truy cập WiFi riêng:
*   **SSID (Tên WiFi)**: `Nhom1_To1`
*   **Mật khẩu**: `12345678`

### 🔗 Quy trình cấu hình & Giám sát:
1.  Kết nối điện thoại hoặc máy tính của bạn với mạng WiFi `Nhom1_To1`.
2.  Mở trình duyệt bất kỳ và truy cập địa chỉ IP mặc định: `192.168.4.1`.
3.  **Trang kết nối WiFi**: Lần đầu truy cập, ESP32 sẽ hiển thị danh sách các mạng WiFi xung quanh. Nhập mật khẩu mạng WiFi nhà bạn để ESP32 chuyển sang chế độ kết nối Internet.
4.  **Trang Dashboard**: Sau khi kết nối thành công, trình duyệt sẽ tự động chuyển hướng đến Dashboard giám sát nội bộ. Giao diện trực quan hiển thị:
    *   **Trạng thái hệ thống**: Hiển thị bảng màu cảnh báo tương ứng (Xanh: An toàn | Vàng/Cam: Cảnh báo khí gas/nhiệt độ | Đỏ nhấp nháy: **🚨 PHÁT HIỆN CHÁY**).
    *   **Thông số cảm biến**: Giá trị thực tế của cảm biến MQ-2, Nhiệt độ (°C), Độ ẩm (%), Tín hiệu cảm biến lửa.
    *   **Thông tin mạng**: Tên WiFi hiện tại đang kết nối và địa chỉ IP được cấp phát.

