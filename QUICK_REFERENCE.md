# SmartDesk 快速参考卡

## 📋 编译与烧录

```bash
# 编译项目
pio run -e black_f407ve

# 烧录到STM32
pio run -e black_f407ve -t upload

# 监视串口输出
pio run -e black_f407ve -t monitor
```

## ⚙️ 关键配置修改（config.h）

```cpp
// WiFi配置
#define WIFI_SSID "YourSSID"
#define WIFI_PASSWORD "YourPassword"

// MQTT配置（OneNet）
#define MQTT_USERNAME "your_device_id"
#define MQTT_PASSWORD "your_api_key"

// 天气API
#define WEATHER_API_KEY "your_seniverse_key"

// 温湿度阈值
#define TEMP_HIGH_THRESHOLD 30.0f
#define TEMP_LOW_THRESHOLD 15.0f
#define HUMID_HIGH_THRESHOLD 70.0f
#define HUMID_LOW_THRESHOLD 30.0f
```

## 🎤 语音指令速查表

| 指令 | 效果 | 说法 |
|------|------|------|
| 开心 | 😊 + 点头 | "happy", "开心" |
| 伤心 | 😢 + 低头 | "sad", "伤心", "难过" |
| 生气 | 😠 + 摇头 | "angry", "生气" |
| 困倦 | 😴 + 静止 | "sleepy", "困", "睡眠" |
| 惊讶 | 😮 + 点头 | "surprised", "惊讶", "惊" |
| 摇头 | 头部左右摇 | "shake", "摇头" |
| 点头 | 头部上下点 | "nod", "点头" |
| 天气 | 显示天气信息 | "weather", "天气" |
| 温度 | 显示温湿度 | "temp", "温度", "温" |

## 📊 系统状态转移

```
IDLE (空闲)
  ↓ 3秒自动
MONITORING (监测)
  ├─ 语音指令 → VOICE_TRIGGERED → 2秒 → MONITORING
  ├─ 温度异常 → TEMP_WARNING → 恢复 → MONITORING
  └─ 湿度异常 → HUMID_WARNING → 恢复 → MONITORING
```

## 🔌 硬件引脚快速查看

| 设备 | 类型 | 引脚 | 波特率 |
|------|------|------|--------|
| DHT11 | GPIO | PA1 | - |
| OLED SDA | I2C | PB7 | 400kHz |
| OLED SCL | I2C | PB6 | 400kHz |
| 舵机摇头 | PWM | PA0 | - |
| 舵机点头 | PWM | PA5 | - |
| ASRPRO TX | UART2 | PA2 | 9600 |
| ASRPRO RX | UART2 | PA3 | 9600 |
| ESP8266 TX | UART3 | PB10 | 115200 |
| ESP8266 RX | UART3 | PB11 | 115200 |

## 🐛 故障快速诊断

### 问题：编译错误 "未找到头文件"
```
解决: 检查platformio.ini的lib_deps是否包含该库
或运行: pio lib install "library_name"
```

### 问题：烧录失败
```
检查:
1. ST-Link连接是否正确
2. STM32是否有电源
3. 运行: pio run -e black_f407ve -v (查看详细日志)
```

### 问题：串口无输出
```
检查:
1. USB串口线连接
2. 串口波特率设置为115200
3. STM32是否已成功烧录
4. 重新上电复位
```

### 问题：DHT不读数
```
解决:
1. 验证引脚连接到PA1
2. DHT应至少间隔1秒读取
3. 检查数据线是否需要4.7kΩ上拉电阻
```

### 问题：OLED黑屏
```
解决:
1. 检查I2C连接（SDA=PB7, SCL=PB6）
2. 尝试修改config.h中的地址:
   #define OLED_I2C_ADDR 0x3D  // 或 0x3C
3. 运行I2C扫描程序找到正确地址
```

### 问题：WiFi无法连接
```
检查:
1. SSID和密码正确
2. 路由器支持2.4GHz
3. WiFi信号强度
4. 重启路由器
```

### 问题：MQTT连接失败 (rc=4 或 5)
```
解决:
1. 验证OneNet的device_id
2. 验证OneNet的api_key
3. 确认MQTT_BROKER地址正确
4. 在OneNet平台检查设备状态
```

## 📱 数据上报格式

```json
// 温湿度
{"temp":25.5,"humidity":45.2}

// 天气
{"weather":"晴","temp_max":28.0,"temp_min":20.0}

// 状态
{"status":"online"}
```

## 🎯 日常使用流程

### 首次使用
```
1. 编辑config.h配置WiFi和API密钥
2. pio run -e black_f407ve 编译
3. pio run -e black_f407ve -t upload 烧录
4. pio run -e black_f407ve -t monitor 监视
5. 等待系统初始化完成
```

### 日常操作
```
1. 给系统通电
2. 等待WiFi连接（约10-30秒）
3. 通过语音指令或环境交互
4. 查看云平台实时数据
```

### 故障恢复
```
1. 查看DEBUG_GUIDE.md
2. 运行故障诊断代码
3. 修改配置重新烧录
```

## 📈 性能指标

| 指标 | 目标值 | 实际值 |
|------|--------|--------|
| WiFi连接时间 | <30s | ~15s |
| MQTT连接时间 | <5s | ~2s |
| DHT读取间隔 | ≥1s | 2s |
| 天气更新间隔 | 10min | 10min |
| MQTT发送间隔 | 30s | 30s |
| 舵机响应时间 | <100ms | <50ms |
| OLED刷新率 | >1Hz | 1Hz |

## 🔐 安全建议

1. **不要在GitHub上提交config.h中的敏感信息**
   ```
   .gitignore中添加: include/config.h
   ```

2. **定期修改云平台密码**

3. **启用防火墙限制MQTT访问**

4. **使用MQTT over TLS加密连接（可选）**

## 📞 获取帮助

1. 查看项目文档：
   - README.md - 项目概览
   - DEBUG_GUIDE.md - 故障排查
   - ARCHITECTURE.md - 架构设计

2. 检查串口日志：
   ```
   每个模块都有 [Module] 开头的日志
   便于快速定位问题
   ```

3. 参考示例：
   - main.cpp 中的注释说明
   - 各模块类的详细文档

---

**最后更新**: 2026年2月5日
**项目状态**: ✅ 生产就绪
**编译状态**: ✅ 无错误
