# SmartDesk 系统调试与初始化指南

## 1. 预编译检查清单

### 硬件连接验证
- [ ] STM32F407VET6已正确连接到编程器
- [ ] DHT11传感器连接到PA1
- [ ] SSD1315 OLED已连接（SDA=PB7, SCL=PB6）
- [ ] 两个SG90舵机分别连接到PA0（摇头）和PA5（点头）
- [ ] ASRPRO语音模块连接到Serial2（PA2/PA3）
- [ ] ESP8266-01S连接到Serial3（PB10/PB11）
- [ ] 所有电源和地线连接正确
- [ ] USB供电或外部电源已连接

### 软件配置验证
在 `config.h` 中检查以下项目：

```cpp
// 1. WiFi配置
#define WIFI_SSID "your_network_name"        // 修改为你的WiFi SSID
#define WIFI_PASSWORD "your_password"        // 修改为你的WiFi密码

// 2. MQTT配置（OneNet）
#define MQTT_USERNAME "device_id"            // OneNet设备ID
#define MQTT_PASSWORD "api_key"              // OneNet API密钥

// 3. 天气API配置
#define WEATHER_API_KEY "YOUR_API_KEY"       // 心知天气API密钥
// 获取方式：https://www.seniverse.com/
```

## 2. 编译步骤

### 方法A: 使用VS Code + PlatformIO扩展
```
1. 在VS Code中打开项目文件夹
2. 点击底部PlatformIO工具栏上的"Build"按钮
3. 等待编译完成，查看编译错误（如有）
4. 修正错误并重新编译
```

### 方法B: 使用命令行
```bash
cd ~/Documents/PlatformIO/Projects/SmartDesk
pio run -e black_f407ve
```

### 常见编译错误处理

**错误**: `DHT.h: No such file or directory`
```bash
# 解决: 安装缺失的库
pio lib install "adafruit/DHT sensor library"
```

**错误**: `U8g2lib.h: No such file or directory`
```bash
# 解决: 安装U8g2库
pio lib install "olikraus/U8g2"
```

## 3. 烧录步骤

### 使用ST-Link v2/v3编程器
```bash
# 1. 连接编程器到PC和STM32
# 2. 运行烧录命令
pio run -e black_f407ve -t upload

# 3. 等待烧录完成，显示 "avrdude done.  Thank you."
```

### 使用JTAG/SWD连接验证
- 接线检查:
  - ST-Link GND → STM32 GND
  - ST-Link SWDIO → STM32 PA13
  - ST-Link SWCLK → STM32 PA14

## 4. 初始化验证

烧录成功后，系统会自动初始化，顺序如下：

### 串口监视器输出预期
```
(系统启动)
================================
Smart Desk Pet System Starting!
STM32F407VET6 + Arduino Framework
================================

[Init] UART initialized
[Init] OLED Display initialized
[Init] Sensors and actuators initialized
[Init] WiFi and Cloud services initialized
[Init] State Machine initialized
[Init] All systems ready!

[WiFi] Connecting to SSID: your_network_name (Attempt 1)
[WiFi] Connected! IP: 192.168.1.xxx
[WiFi] Signal strength: -45

[MQTT] Attempting to connect...
[MQTT] Connected successfully!
[MQTT] Subscribed to: $dp/cmd/your_device_id

[Sensor] T=25.5C, H=45.2%
[Sensor] T=25.6C, H=45.1%
...
```

### 显示屏验证
1. 开机应显示正常表情（😐）
2. 显示进度信息变化
3. 2秒后显示实时温湿度

### 舵机验证
1. 开机时舵机会回到中点（90°）
2. 无异常声音或抖动

## 5. 功能测试

### 传感器测试
```
1. 靠近/远离DHT11
   - 观察串口输出温湿度变化
   - 显示屏实时更新值

2. 加热/冷却
   - 温度 > 30°C 时→ 显示热警告，舵机摇头
   - 温度 < 15°C 时→ 显示冷警告，舵机摇头
```

### WiFi & MQTT测试
```
1. 检查WiFi连接
   - 串口应显示 "Connected! IP: xxx"
   
2. 检查MQTT连接
   - 串口应显示 "MQTT Connected successfully!"
   
3. 在OneNet平台查看数据
   - 温湿度数据每30秒上报一次
   - 天气数据每10分钟更新一次
```

### 语音识别测试
```
对着ASRPRO模块说出指令：
- "开心" → 应显示开心表情+点头
- "伤心" → 应显示伤心表情+低头
- "摇头" → 舵机应执行摇头动作
- "天气" → 显示天气信息
```

### 舵机动作测试
直接通过语音指令触发所有舵机动作：
```
"开心" / "惊讶" → NOD_UP（点头上抬）
"伤心" → NOD_DOWN（点头低下）
"生气" / "摇头" → SHAKE_LEFT（左转）
"帮助" → SHAKE_RIGHT（右转）
```

## 6. 故障排查

### OLED不显示任何内容

**诊断**:
```cpp
// 在main.cpp中的setupDisplay()后添加调试代码
Serial.println(u8g2.begin() ? "OLED OK" : "OLED FAILED");
```

**可能原因**:
- I2C地址不匹配（默认0x3C，某些模块为0x3D）
- 电源不足
- I2C连线错误

**解决方案**:
```cpp
// 在config.h中尝试修改
#define OLED_I2C_ADDR 0x3D  // 如果默认0x3C不行，试试0x3D
```

### DHT11读取失败

**症状**: 串口显示 "DHT11 read failed!" 反复出现

**原因**:
- 连线接触不良
- 数据线需要上拉电阻（推荐4.7kΩ）
- 读取间隔过短（DHT11最小间隔~1秒）

**解决**:
```cpp
// config.h中检查
#define SENSOR_READ_INTERVAL 2000  // 确保间隔足够长
```

### WiFi无法连接

**症状**: 反复显示 "Connecting to SSID..."

**原因**:
- WiFi SSID或密码错误
- 2.4GHz WiFi信号弱或不支持该协议
- ESP8266固件需要升级

**解决**:
1. 检查 `config.h` 中WIFI_SSID和WIFI_PASSWORD
2. 确保WiFi路由器支持2.4GHz
3. 尝试重启路由器

### MQTT连接失败

**症状**: "MQTT Connection failed, rc=X"

**rc值含义**:
- rc=-4: 连接超时
- rc=-3: 连接丢失
- rc=-2: 连接失败
- rc=-1: 服务器无响应
- rc=1: 不兼容的协议版本
- rc=2: 无效的客户端ID
- rc=3: 服务器不可用
- rc=4: 用户名或密码错误
- rc=5: 认证失败

**解决**:
1. 检查OneNet的device_id和api_key
2. 确保WiFi已连接
3. 检查MQTT Broker地址和端口

### 舵机不动作

**症状**: 串口正常，但舵机无反应

**原因**:
- PWM引脚配置错误
- 舵机电源不足
- 舵机已损坏

**诊断**:
```cpp
// 在loop()中添加测试代码
servoController.setHeadPosition(180);
delay(1000);
servoController.setHeadPosition(0);
delay(1000);
servoController.setHeadPosition(90);
```

## 7. 性能监测

### 内存使用
```cpp
// 在loop()中定期打印内存使用
Serial.print("Free memory: ");
Serial.println(freeMemory());  // 需要添加freeMemory()函数
```

### 任务执行时间
```cpp
// 测量某个操作的耗时
unsigned long start = micros();
// ... 要测量的代码 ...
unsigned long elapsed = micros() - start;
Serial.print("Execution time: ");
Serial.print(elapsed);
Serial.println(" us");
```

## 8. 生产部署检查清单

- [ ] 所有传感器数据正常
- [ ] WiFi连接稳定（信号强度 > -70dBm）
- [ ] MQTT连接保持在线
- [ ] 舵机动作顺畅无卡顿
- [ ] OLED显示清晰无花屏
- [ ] 语音识别准确率 > 90%
- [ ] 温湿度数据可靠地上报到云平台
- [ ] 系统运行72小时无重启

## 9. 常用调试代码片段

### 打印系统状态
```cpp
void printSystemStatus() {
    Serial.println("=== System Status ===");
    Serial.print("WiFi: ");
    Serial.println(wifiManager.isConnected() ? "OK" : "FAILED");
    Serial.print("MQTT: ");
    Serial.println(mqttManager.isConnectedToMQTT() ? "OK" : "FAILED");
    Serial.print("DHT: ");
    Serial.println(dhtManager.getIsValid() ? "OK" : "FAILED");
    Serial.print("FSM State: ");
    Serial.println((int)fsm.getCurrentState());
}
```

### 临时禁用WiFi进行本地测试
```cpp
// 在config.h中
#define DISABLE_WIFI 1

// 在main.cpp的setupWiFiAndCloud()中
#ifdef DISABLE_WIFI
Serial.println("[DEBUG] WiFi disabled for local testing");
// 跳过WiFi初始化
#endif
```

## 10. 获取技术支持

如遇到问题无法解决，请收集以下信息：
1. 串口监视器的完整输出日志
2. 硬件连接照片
3. config.h中的配置信息（隐藏敏感数据）
4. 遇到的具体错误现象描述
