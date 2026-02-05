# 硬件引脚快速对照表

## 📌 STM32F407VET6 引脚使用总览

```
                    STM32F407VET6 (168MHz)
                          |
        ┌─────────────────┼─────────────────┐
        │                 │                 │
    UART2 (ASRPRO)   I2C1 (OLED)      UART3 (ESP8266)
    TX=PA2/RX=PA3   SCL=PB6/SDA=PB7   TX=PB10/RX=PB11
    9600 bps        128x64 SSD1315    115200 bps
        │                 │                 │
        └─────────────────┼─────────────────┘
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
      GPIO            GPIO            PWM/GPIO
      PA1 (DHT11)   PA0 (Servo)      PA5 (Servo)
      Single-Wire   摇头 180°         点头 180°
        │                 │                 │
```

---

## 🔌 完整引脚映射表

### Group A: 模拟与数字I/O

| 引脚 | 功能 | 模块 | 配置 | 协议 | 状态 |
|------|------|------|------|------|------|
| **PA0** | Servo Head | SG90舵机1 | PWM输出 | PWM | ✅ |
| **PA1** | DHT Data | DHT11 | 数字I/O | Single-Wire | ✅ |
| **PA2** | ASRPRO TX | 语音识别 | UART输出 | UART2 | ✅ |
| **PA3** | ASRPRO RX | 语音识别 | UART输入 | UART2 | ✅ |
| **PA5** | Servo Nod | SG90舵机2 | PWM输出 | PWM | ✅ |

### Group B: I2C总线

| 引脚 | 功能 | 模块 | 配置 | 协议 | 状态 |
|------|------|------|------|------|------|
| **PB6** | I2C1_SCL | OLED | 开漏输出 | I2C | ✅ |
| **PB7** | I2C1_SDA | OLED | 开漏输出 | I2C | ✅ |

### Group C: UART3 (联网模块)

| 引脚 | 功能 | 模块 | 配置 | 协议 | 状态 |
|------|------|------|------|------|------|
| **PB10** | UART3_TX | ESP8266 | UART输出 | UART3 | ✅ |
| **PB11** | UART3_RX | ESP8266 | UART输入 | UART3 | ✅ |

---

## 🎯 硬件模块配置一览

### 1️⃣ 显示屏 (SSD1315 OLED)

```
型号:     SSD1315
大小:     0.96寸
分辨率:   128x64像素
通信:     I2C
地址:     0x3C
SCL:      PB6  ← [STM32F407VET6]
SDA:      PB7  ← [STM32F407VET6]
功能:     表情显示、温湿度显示、天气显示
```

**代码配置** (config.h):
```cpp
#define OLED_SDA PB7
#define OLED_SCL PB6
#define OLED_I2C_ADDR 0x3C
```

---

### 2️⃣ 温湿度传感器 (DHT11)

```
型号:     DHT11
通信:     Single-Wire (GPIO)
引脚:     PA1  ← [STM32F407VET6]
精度:     温度±2°C, 湿度±5%RH
功能:     环境监测、温度告警
```

**代码配置** (config.h):
```cpp
#define DHT_PIN PA1
#define DHT_TYPE DHT11
```

---

### 3️⃣ 语音识别模块 (ASRPRO)

```
型号:     ASRPRO
通信:     UART (Serial2)
波特率:   9600 bps
TX:       PA2  ← [STM32F407VET6]
RX:       PA3  ← [STM32F407VET6]
功能:     离线语音识别、命令解析
电源:     5V + 降压至3.3V
```

**代码配置** (config.h):
```cpp
#define UART_ASRPRO Serial2
#define UART_BAUD_ASRPRO 9600
```

---

### 4️⃣ WiFi模块 (ESP8266-01S)

```
型号:     ESP8266-01S
通信:     UART (Serial3)
波特率:   115200 bps
TX:       PB10  ← [STM32F407VET6]
RX:       PB11  ← [STM32F407VET6]
功能:     WiFi连接、MQTT通信、云服务
电源:     3.3V (独立供电)
```

**代码配置** (config.h):
```cpp
#define UART_ESP8266 Serial3
#define UART_BAUD_ESP8266 115200
#define MQTT_BROKER "mqtt.heclouds.com"
#define MQTT_PORT 1883
```

---

### 5️⃣ 舵机1 - 头部水平 (SG90)

```
型号:     SG90
控制:     PWM (50Hz)
引脚:     PA0  ← [STM32F407VET6]
范围:     0-180度
功能:     头部左右摇动
电源:     5V (独立供电, 最大1-2A)
```

**代码配置** (config.h):
```cpp
#define SERVO_HEAD_PIN PA0
```

---

### 6️⃣ 舵机2 - 头部垂直 (SG90)

```
型号:     SG90
控制:     PWM (50Hz)
引脚:     PA5  ← [STM32F407VET6]
范围:     0-180度
功能:     头部上下点头
电源:     5V (独立供电, 最大1-2A)
```

**代码配置** (config.h):
```cpp
#define SERVO_NOD_PIN PA5
```

---

## 📊 通信总线速率汇总

| 总线 | 协议 | 速率 | 引脚 | 状态 |
|------|------|------|------|------|
| **I2C1** | I2C | 100/400kHz | PB6/PB7 | ✅ |
| **UART2** | Serial | 9600 bps | PA2/PA3 | ✅ |
| **UART3** | Serial | 115200 bps | PB10/PB11 | ✅ |

---

## ⚡ 电源分配表

| 设备 | 电压 | 电流 | 备注 |
|------|------|------|------|
| STM32F407VET6 | 3.3V | 500mA | 主控 |
| OLED SSD1315 | 3.3V | 50mA | 显示屏 |
| DHT11 | 3.3V | 10mA | 传感器 |
| ASRPRO | 5V | 200mA | 需降压至3.3V |
| ESP8266-01S | 3.3V | 500mA | 独立供电 |
| Servo1 (PA0) | 5V | 1-2A | 独立供电 |
| Servo2 (PA5) | 5V | 1-2A | 独立供电 |

**总电流**: ~5-6A (峰值) / 1-2A (平均)

---

## 🔍 引脚冲突检查

```
✅ 无I2C冲突  (I2C1独占PB6/PB7)
✅ 无UART冲突 (Serial2: PA2/PA3, Serial3: PB10/PB11)
✅ 无GPIO冲突 (PA0, PA1, PA5各自独立)
✅ 无PWM冲突  (PA0, PA5独占各自的定时器通道)
```

---

## 🛠️ 调试建议

### 调试UART (推荐使用Serial1)

```
Serial1: PA9 (TX) / PA10 (RX)
波特率: 115200 bps
用途: 实时日志输出和调试
```

**主程序中的使用**:
```cpp
void setup() {
    Serial.begin(115200);           // Serial1: PA9/PA10
    UART_ASRPRO.begin(9600);        // Serial2: PA2/PA3
    UART_ESP8266.begin(115200);     // Serial3: PB10/PB11
}
```

---

## 📋 config.h 快速查阅

### I2C配置
```cpp
#define OLED_SDA PB7
#define OLED_SCL PB6
#define OLED_I2C_ADDR 0x3C
```

### GPIO配置
```cpp
#define DHT_PIN PA1
#define SERVO_HEAD_PIN PA0
#define SERVO_NOD_PIN PA5
```

### UART配置
```cpp
#define UART_ASRPRO Serial2      // PA2(TX), PA3(RX), 9600
#define UART_ESP8266 Serial3     // PB10(TX), PB11(RX), 115200
#define UART_BAUD_ASRPRO 9600
#define UART_BAUD_ESP8266 115200
```

---

## ✅ 引脚验证检查清单

- [x] PA0: Servo Head (PWM)
- [x] PA1: DHT11 (GPIO)
- [x] PA2: ASRPRO TX (UART2)
- [x] PA3: ASRPRO RX (UART2)
- [x] PA5: Servo Nod (PWM)
- [x] PB6: OLED SCL (I2C1)
- [x] PB7: OLED SDA (I2C1)
- [x] PB10: ESP8266 TX (UART3)
- [x] PB11: ESP8266 RX (UART3)

**总体状态**: ✅ **全部正确**

---

## 🎯 快速故障排查

| 问题 | 可能原因 | 检查项 |
|------|--------|--------|
| OLED不显示 | I2C通信故障 | PB6/PB7引脚、0x3C地址 |
| DHT读取失败 | GPIO故障 | PA1引脚、信号线 |
| ASRPRO无反应 | UART故障 | PA2/PA3引脚、9600波特率 |
| ESP8266连接失败 | UART故障 | PB10/PB11引脚、115200波特率 |
| 舵机不转 | PWM故障 | PA0/PA5引脚、50Hz频率 |

---

**最后更新**: 2026年2月5日  
**状态**: ✅ **所有引脚配置正确**  
**合规性**: **100% 符合规格表**
