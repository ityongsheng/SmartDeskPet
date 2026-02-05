# 智能桌宠系统 - 项目文档

## 项目概述
基于 STM32F407VET6 微控制器的智能桌宠系统，集成多种传感器、执行器和云连接功能，采用模块化设计和面向对象编程思想。

## 硬件配置

### 主控单元
- **MCU**: STM32F407VET6
- **框架**: Arduino (PlatformIO)

### 传感器与执行器
| 设备 | 型号 | 接口 | 备注 |
|------|------|------|------|
| 温湿度 | DHT11 | GPIO PA1 | 单总线 |
| 显示屏 | SSD1315 OLED | I2C (SDA=PB7, SCL=PB6) | 128x64 |
| 舵机-摇头 | SG90 | PWM PA0 | 180° |
| 舵机-点头 | SG90 | PWM PA5 | 180° |
| 语音识别 | ASRPRO | Serial2 (TX=PA2, RX=PA3) | 9600 baud |
| 物联网 | ESP8266-01S | Serial3 (TX=PB10, RX=PB11) | 115200 baud |

## 软件架构

### 分层设计

```
┌─────────────────────────────────────┐
│     应用层 (main.cpp)               │
│   核心业务逻辑、初始化、主循环      │
└─────────────────────────────────────┘
         ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓
┌─────────────────────────────────────┐
│     交互层 (State_Machine.h/cpp)    │
│   有限状态机、事件处理、决策逻辑    │
└─────────────────────────────────────┘
         ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓
┌─────────────────────────────────────┐
│  通信层  │      驱动层                │
├──────────┼───────────────────────────┤
│• WiFi    │• DHT_Manager              │
│• MQTT    │• OLED_Display             │
│• Weather │• Servo_Controller         │
│          │• ASRPRO_Module            │
└─────────────────────────────────────┘
```

## 模块详解

### 1. 驱动层 (Driver Layer)

#### DHT_Manager（温湿度管理）
- **功能**: 读取DHT11传感器，进行校验，触发环境阈值
- **关键方法**:
  - `read()`: 读取温湿度值
  - `getTemperature()`, `getHumidity()`: 获取缓存值
  - `isTempHigh()`, `isTempLow()`: 温度阈值检查
  - `isHumidityHigh()`, `isHumidityLow()`: 湿度阈值检查

#### OLED_Display（显示屏管理）
- **功能**: 驱动SSD1315 OLED，显示表情和信息
- **表情类型**: HAPPY, SAD, ANGRY, SLEEPY, SURPRISED, NORMAL, HOT_WARNING, COLD_WARNING, HUMID_WARNING
- **关键方法**:
  - `setEmotion(EmotionState)`: 切换表情
  - `displayTempHumidity()`: 显示温湿度
  - `displayWeather()`: 显示天气信息
  - `displayIP()`: 显示网络IP

#### Servo_Controller（舵机控制）
- **功能**: 控制两个SG90舵机（摇头和点头）
- **动作类型**: SHAKE_LEFT, SHAKE_RIGHT, NOD_UP, NOD_DOWN, RESET
- **关键方法**:
  - `performAction()`: 执行舵机动作
  - `update()`: 非阻塞更新（自动复位）
  - `setHeadPosition()`, `setNodPosition()`: 直接设置舵机角度

#### ASRPRO_Module（语音识别）
- **功能**: 处理语音识别指令
- **支持指令**: 
  - 表情类: "开心", "伤心", "生气", "困倦", "惊讶"
  - 动作类: "摇头", "点头", "摇"
  - 查询类: "天气", "温度", "帮助"
- **关键方法**:
  - `update()`: 读取串口数据
  - `getLastCommand()`: 获取识别的指令

### 2. 通信层 (Communication Layer)

#### WiFi_Manager（WiFi管理）
- **功能**: 管理ESP8266 WiFi连接，自动重连
- **重连策略**: 指数退避（1s, 2s, 4s, 8s, ..., 60s）
- **关键方法**:
  - `connect()`: 连接WiFi
  - `update()`: 检查连接状态
  - `getLocalIP()`: 获取本地IP
  - `getSignalStrength()`: 获取信号强度

#### MQTT_Manager（MQTT通信）
- **对接平台**: OneNet 云平台
- **数据格式**: JSON
- **关键方法**:
  - `connect()`: 连接MQTT Broker
  - `publishTempHumidity()`: 发布温湿度
  - `publishWeather()`: 发布天气信息
  - `publishStatus()`: 发布设备状态
  - `subscribe()`: 订阅主题进行远程控制

#### Weather_Service（天气服务）
- **数据源**: 心知天气 API (api.seniverse.com)
- **更新间隔**: 10分钟
- **关键方法**:
  - `updateWeather()`: 从API获取天气数据
  - `parseWeatherJSON()`: 解析JSON响应

### 3. 交互层 (Interaction Layer)

#### State_Machine（有限状态机）

**系统状态**:
- `IDLE`: 空闲初始化状态
- `MONITORING`: 监测环境，定期读取传感器
- `VOICE_TRIGGERED`: 语音指令触发，执行动作
- `TEMP_WARNING`: 温度异常警告
- `HUMID_WARNING`: 湿度异常警告
- `WEATHER_DISPLAY`: 显示天气信息

**状态转移流程**:
```
    ┌─────────┐
    │  IDLE   │
    └────┬────┘
         │ 3s
         ↓
    ┌──────────────┐
    │  MONITORING  │←─────────┐
    └┬─┬─┬─────┬──┘          │
     │ │ │     │             │
     │ │ │     └─ 恢复正常 ──┘
     │ │ │
     │ │ └─→ HUMID_WARNING
     │ │
     │ └───→ TEMP_WARNING
     │
     └─────→ VOICE_TRIGGERED
```

## 配置文件说明

### config.h
包含所有硬件配置、API密钥、阈值设置：

```cpp
// 引脚定义
#define DHT_PIN PA1
#define OLED_SDA PB7
#define OLED_SCL PB6
#define SERVO_HEAD_PIN PA0
#define SERVO_NOD_PIN PA5

// 温湿度阈值
#define TEMP_HIGH_THRESHOLD 30.0f
#define TEMP_LOW_THRESHOLD 15.0f
#define HUMID_HIGH_THRESHOLD 70.0f
#define HUMID_LOW_THRESHOLD 30.0f

// WiFi & MQTT
#define WIFI_SSID "your_ssid"
#define MQTT_BROKER "mqtt.heclouds.com"

// 天气API
#define WEATHER_API_KEY "YOUR_API_KEY"
```

## 编译与烧录

### 使用PlatformIO CLI
```bash
# 编译
pio run -e black_f407ve

# 烧录
pio run -e black_f407ve -t upload

# 监控串口
pio run -e black_f407ve -t monitor
```

### 使用VS Code + PlatformIO插件
- 点击"Build"编译
- 点击"Upload"烧录
- 点击"Serial Monitor"查看输出

## 使用指南

### 快速开始
1. 修改 `config.h` 中的WiFi和API密钥配置
2. 编译并烧录到STM32F407VET6
3. 通过串口监视器查看初始化过程
4. 系统自动连接WiFi和MQTT
5. 使用语音指令或环境触发自动交互

### 语音指令示例
```
"开心" → 开心表情 + 点头动作
"伤心" → 伤心表情 + 低头动作
"生气" → 生气表情 + 摇头动作
"天气" → 获取并显示天气信息
"温度" → 显示当前温湿度
```

### 环境触发示例
- 温度 > 30°C → 显示热警告表情 + 摇头
- 温度 < 15°C → 显示冷警告表情 + 摇头
- 湿度 > 70% 或 < 30% → 显示湿度警告表情

## 异常处理

### WiFi重连机制
- 采用指数退避算法避免频繁重试
- 最多重试10次，间隔最大60秒
- 连接失败时在OLED显示提示

### 传感器校验
- DHT11数据有效性检查（isnan）
- 温湿度值范围校验
- 读取失败自动重试

### MQTT保活机制
- 定期发送心跳包（30秒）
- 连接失败自动重连
- 数据发送前检查连接状态

## 扩展建议

1. **增加传感器**: 继承驱动层基类，新增传感器管理类
2. **扩展语音命令**: 在ASRPRO_Module中添加parseCommand分支
3. **自定义表情**: 在OLED_Display中编写新的drawXxx()方法
4. **添加其他云服务**: 创建新的通信管理类（如ThingSpeak、Azure等）

## 故障排查

| 问题 | 原因 | 解决方案 |
|------|------|--------|
| OLED不显示 | I2C地址错误 | 检查config.h中OLED_I2C_ADDR |
| DHT读取失败 | 连接不稳定 | 检查引脚连接和数据线 |
| WiFi无法连接 | SSID/密码错误 | 在config.h修改WIFI_SSID和WIFI_PASSWORD |
| MQTT连接失败 | 设备ID或API Key错误 | 检查OneNet平台配置 |

## 许可证
MIT License

## 联系方式
SmartDesk Project Team
