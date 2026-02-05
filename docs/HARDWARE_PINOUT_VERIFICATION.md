# SmartDesk 硬件引脚与型号规格检查报告

**检查日期**: 2026年2月5日  
**项目**: SmartDesk - 多模态智能互动系统  
**平台**: STM32F407VET6  
**状态**: ✅ 全部通过检查

---

## 📋 硬件配置规格表

| 模块名称 | 型号/规格 | 通信协议 | 建议引脚 (STM32F407VET6) | 当前配置 | 状态 |
|---------|---------|--------|----------------------|--------|------|
| 主控 | STM32F407VET6 | - | - | ✅ | ✅ |
| 显示屏 | SSD1315 (0.96寸) | I2C | SCL: PB6, SDA: PB7 | SCL: PB6, SDA: PB7 | ✅ |
| 传感器 | DHT11 | Single-Wire | PA1 | PA1 | ✅ |
| 语音 | ASRPRO | UART (Serial2) | TX: PA2, RX: PA3 | TX: PA2, RX: PA3 | ✅ |
| 联网 | ESP8266-01S | UART (Serial3) | TX: PB10, RX: PB11 | TX: PB10, RX: PB11 | ✅ |
| 舵机1 | SG90 (180度) | PWM | PA0 | PA0 | ✅ |
| 舵机2 | SG90 (180度) | PWM | PA5 | PA5 | ✅ |

---

## ✅ 详细检查结果

### 1. 主控器 (STM32F407VET6)

**规格**: STM32F407VET6 (168MHz, 192KB SRAM, 512KB Flash)  
**检查项**:
- ✅ 核心控制芯片确认正确
- ✅ 时钟频率 168MHz
- ✅ 内存容量充足

---

### 2. 显示屏 (SSD1315 OLED)

**规格**: 0.96寸 OLED，分辨率 128x64，I2C接口  
**地址**: 0x3C

**引脚配置**:
```
I2C 接口:
  SCL (时钟线):  PB6  ✅
  SDA (数据线):  PB7  ✅
  地址:         0x3C  ✅
```

**代码验证**:
```cpp
// config.h 第9-11行
#define OLED_SDA PB7
#define OLED_SCL PB6
#define OLED_I2C_ADDR 0x3C
```

**驱动验证** (OLED_Display.h):
```cpp
// 第30行
U8G2_SSD1315_128X64_1_HW_I2C u8g2;
```

**状态**: ✅ **完全正确**

---

### 3. 温湿度传感器 (DHT11)

**规格**: DHT11 温湿度一体化传感器，Single-Wire接口

**引脚配置**:
```
数据线:  PA1  ✅
```

**代码验证**:
```cpp
// config.h 第6-7行
#define DHT_PIN PA1
#define DHT_TYPE DHT11
```

**驱动验证** (DHT_Manager.h):
```cpp
// 使用 DHT.h 库进行初始化
DHT dht;
void begin();
```

**状态**: ✅ **完全正确**

---

### 4. 语音识别模块 (ASRPRO)

**规格**: ASRPRO 离线语音识别模块，UART通信

**引脚配置**:
```
UART 接口 (Serial2):
  TX:  PA2      ✅
  RX:  PA3      ✅
波特率: 9600   ✅
```

**代码验证**:
```cpp
// config.h 第18-21行
#define UART_ASRPRO Serial2           // UART2 = PA2(TX), PA3(RX)
#define UART_BAUD_ASRPRO 9600
```

**驱动初始化** (main.cpp 第39-40行):
```cpp
UART_ASRPRO.begin(UART_BAUD_ASRPRO);  // Serial2 begin
```

**状态**: ✅ **完全正确**

---

### 5. 联网模块 (ESP8266-01S)

**规格**: ESP8266-01S WiFi模块，UART通信，连接OneNet

**引脚配置**:
```
UART 接口 (Serial3):
  TX:  PB10     ✅
  RX:  PB11     ✅
波特率: 115200 ✅
```

**代码验证**:
```cpp
// config.h 第22-23行
#define UART_ESP8266 Serial3          // UART3 = PB10(TX), PB11(RX)
#define UART_BAUD_ESP8266 115200
```

**驱动初始化** (main.cpp 第40行):
```cpp
UART_ESP8266.begin(UART_BAUD_ESP8266);  // Serial3 begin
```

**MQTT配置** (config.h 第33-38行):
```cpp
#define MQTT_BROKER "mqtt.heclouds.com"
#define MQTT_PORT 1883
#define MQTT_USERNAME "your_device_id"
```

**状态**: ✅ **完全正确**

---

### 6. 舵机1 - 头部水平转动 (SG90)

**规格**: SG90伺服电机，180度转动，PWM控制

**引脚配置**:
```
PWM 接口:
  引脚:  PA0      ✅
  功能:  摇头     ✅
  范围:  0-180°   ✅
```

**代码验证**:
```cpp
// config.h 第13-14行
#define SERVO_HEAD_PIN PA0      // 摇头舵机
```

**驱动验证** (Servo_Controller.h 第25-26行):
```cpp
Servo headServo;    // 摇头舵机
Servo nodServo;     // 点头舵机
```

**状态**: ✅ **完全正确**

---

### 7. 舵机2 - 头部垂直点头 (SG90)

**规格**: SG90伺服电机，180度转动，PWM控制

**引脚配置**:
```
PWM 接口:
  引脚:  PA5      ✅
  功能:  点头     ✅
  范围:  0-180°   ✅
```

**代码验证**:
```cpp
// config.h 第13-14行
#define SERVO_NOD_PIN PA5       // 点头舵机
```

**驱动验证** (Servo_Controller.h 中使用两个Servo对象):
```cpp
void performAction(ServoAction action);
```

**状态**: ✅ **完全正确**

---

## 🔍 STM32F407VET6 引脚映射验证

### UART 引脚确认

```
Serial2 (UART2):
  TX:  PA2  ✅ (连接 ASRPRO)
  RX:  PA3  ✅ (连接 ASRPRO)

Serial3 (UART3):
  TX:  PB10 ✅ (连接 ESP8266)
  RX:  PB11 ✅ (连接 ESP8266)
```

### I2C 引脚确认

```
I2C1:
  SCL: PB6  ✅ (连接 OLED SSD1315)
  SDA: PB7  ✅ (连接 OLED SSD1315)
```

### GPIO 引脚确认

```
Single-Wire/GPIO:
  PA1: DHT11         ✅

PWM/GPIO:
  PA0: Servo Head    ✅
  PA5: Servo Nod     ✅
```

---

## 📋 config.h 完整引脚定义

```cpp
// 显示屏 I2C 引脚
#define OLED_SDA PB7        ✅
#define OLED_SCL PB6        ✅

// 传感器引脚
#define DHT_PIN PA1         ✅

// 舵机引脚
#define SERVO_HEAD_PIN PA0  ✅
#define SERVO_NOD_PIN PA5   ✅

// UART 定义
#define UART_ASRPRO Serial2    // TX=PA2, RX=PA3 ✅
#define UART_ESP8266 Serial3   // TX=PB10, RX=PB11 ✅
```

---

## ⚠️ 注意事项

### 1. UART通信波特率
- ASRPRO (Serial2): **9600 bps** ✅
- ESP8266 (Serial3): **115200 bps** ✅

### 2. I2C通信频率
- OLED (I2C1): **标准模式 (100kHz)** 或 **快速模式 (400kHz)**
- 地址: **0x3C** ✅

### 3. PWM控制
- Servo Head (PA0): **50Hz PWM**, 1-2ms 脉宽
- Servo Nod (PA5): **50Hz PWM**, 1-2ms 脉宽
- Arduino Servo库自动处理频率

### 4. 电源管理
- DHT11: 3.3V 供电
- OLED: 3.3V 供电
- ASRPRO: 需要独立5V供电 (带降压模块)
- ESP8266: 需要独立3.3V供电 (最大电流500mA)
- Servo: 需要独立5V供电 (每个最大1-2A)

---

## 🔧 platformio.ini 配置检查

```ini
[env:black_f407ve]
platform = ststm32
board = black_f407ve
framework = arduino
```

**STM32F407VET6 (blackpill) 支持**:
- ✅ Serial1 (PA9/PA10) - 调试
- ✅ Serial2 (PA2/PA3) - ASRPRO
- ✅ Serial3 (PB10/PB11) - ESP8266
- ✅ I2C1 (PB6/PB7) - OLED
- ✅ PWM on PA0, PA5
- ✅ GPIO on PA1

---

## 📊 引脚使用汇总表

| 引脚 | 功能 | 协议 | 状态 |
|------|------|------|------|
| PA0 | Servo Head (PWM) | PWM | ✅ |
| PA1 | DHT11 (数据) | GPIO/Single-Wire | ✅ |
| PA2 | ASRPRO (TX) | UART2 | ✅ |
| PA3 | ASRPRO (RX) | UART2 | ✅ |
| PA5 | Servo Nod (PWM) | PWM | ✅ |
| PB6 | OLED (SCL) | I2C1 | ✅ |
| PB7 | OLED (SDA) | I2C1 | ✅ |
| PB10 | ESP8266 (TX) | UART3 | ✅ |
| PB11 | ESP8266 (RX) | UART3 | ✅ |

---

## ✅ 最终检查结论

### 总体评分: **PASS ✅**

**检查项目**:
- ✅ 所有引脚配置正确
- ✅ 所有通信协议正确
- ✅ 所有波特率正确
- ✅ 所有I2C地址正确
- ✅ 代码与硬件规格表完全匹配
- ✅ 无引脚冲突
- ✅ 无通信冲突

**合规性**: **100% 符合规格表**

---

## 🎯 建议

### 硬件连接建议

1. **电源分离**
   - 主芯片 3.3V
   - ASRPRO: 需要5V + 降压模块输出3.3V
   - ESP8266: 独立3.3V供电 (推荐1A以上)
   - Servo: 独立5V供电 (推荐2A以上)

2. **信号完整性**
   - 使用屏蔽线连接UART
   - ASRPRO TX/RX添加100nF去耦电容
   - 舵机PWM线与电源线分离

3. **调试方便**
   - 保留PA9/PA10用于Serial1调试
   - 推荐在主控端添加UART转USB模块

---

## 📄 检查清单

- [x] 主控芯片规格正确
- [x] 显示屏引脚正确 (I2C)
- [x] DHT11引脚正确
- [x] ASRPRO UART引脚正确
- [x] ESP8266 UART引脚正确
- [x] 舵机1 PWM引脚正确
- [x] 舵机2 PWM引脚正确
- [x] 所有代码配置与规格表一致
- [x] 无引脚冲突
- [x] 波特率配置正确

---

**检查员**: Code Agent  
**检查日期**: 2026年2月5日  
**报告版本**: 1.0  
**状态**: ✅ **全部通过**

---

## 关键代码片段验证

### config.h 中的完整引脚定义

```cpp
// ==================== 引脚定义 ====================
// DHT11 温湿度传感器
#define DHT_PIN PA1                  // ✅
#define DHT_TYPE DHT11

// OLED 显示屏 (I2C)
#define OLED_SDA PB7                 // ✅
#define OLED_SCL PB6                 // ✅
#define OLED_I2C_ADDR 0x3C

// 舵机
#define SERVO_HEAD_PIN PA0           // ✅
#define SERVO_NOD_PIN PA5            // ✅

// 串口定义
#define UART_ASRPRO Serial2          // TX=PA2, RX=PA3 ✅
#define UART_ESP8266 Serial3         // TX=PB10, RX=PB11 ✅
#define UART_BAUD_ASRPRO 9600        // ✅
#define UART_BAUD_ESP8266 115200     // ✅
```

**所有配置项状态**: ✅ **100% 符合规格**

---

**SmartDesk 硬件配置检查完成！所有引脚和型号均符合规格表要求。** 🎉
