# SmartDesk 架构设计文档

## 系统架构总览

```
┌──────────────────────────────────────────────────────────────────┐
│                    main.cpp (应用入口)                            │
│  • setup()   - 系统初始化                                         │
│  • loop()    - 主循环，协调各模块                                │
└──────────────────────────────────────────────────────────────────┘
           │                                                    
           ├─────────────────────────────────────────────────┐
           │                                                  │
      ┌────▼──────────────┐                  ┌────────────────▼──┐
      │ State_Machine     │                  │ 其他模块的更新    │
      │ (FSM心脏)        │                  │                   │
      │                  │                  │ • readSensors()   │
      │ 主要职责:        │                  │ • updateWiFi()    │
      │ • 状态管理       │                  │ • updateMQTT()    │
      │ • 事件处理       │                  │ • updateWeather() │
      │ • 决策与驱动     │                  │                   │
      └────┬──────────────┘                  └───────────────────┘
           │
      ┌────┴────────────────────────────────────────────────────┐
      │               状态机触发的模块                          │
      │                                                         │
  ┌───▼──────────┐  ┌──────────────┐  ┌──────────────┐        │
  │ OLEDDisplay  │  │ServoController│  │ DHTManager   │        │
  │              │  │               │  │              │        │
  │• 设置表情    │  │• 执行动作     │  │• 读取传感器  │        │
  │• 显示文字    │  │• 非阻塞更新   │  │• 检查阈值    │        │
  └───┬──────────┘  └──────┬────────┘  └──────┬───────┘        │
      │                    │                   │                │
      └────────────────────┴───────────────────┘                │
      │ 触发器:                                 │
      │ • Voice commands                       │
      │ • Environmental conditions             │
      └─────────────────────────────────────────┘

           ┌─────────────────────────────────┐
           │  ASRPROModule (语音输入)        │
           │  • 读取语音指令                 │
           │  • 解析指令                     │
           │  • 返回VoiceCommand             │
           └─────────────────────────────────┘

      ┌──────────────┬──────────────┬──────────────┐
      │              │              │              │
  ┌───▼────────┐ ┌──▼────────┐ ┌───▼────────┐   │
  │WiFiManager │ │MQTTManager│ │ WeatherSrv │   │
  │            │ │           │ │            │   │
  │• 连接WiFi  │ │• MQTT通信 │ │• 天气数据  │   │
  │• 重连机制  │ │• 数据上报 │ │• JSON解析  │   │
  └────────────┘ └───────────┘ └────────────┘   │
      │ 通信层模块                               │
      └───────────────────────────────────────────┘
```

## 类设计详解

### 1. 驱动层类设计

#### DHT_Manager 类
```cpp
class DHTManager {
  private:
    DHT dht;                          // DHT11对象
    float lastTemp, lastHumidity;     // 缓存值
    unsigned long lastReadTime;       // 上次读取时间
    bool isValid;                     // 数据有效性标志
  
  public:
    bool read();                      // 读取数据（带时间间隔保护）
    float getTemperature() const;     // 获取温度
    float getHumidity() const;        // 获取湿度
    bool isTempHigh() const;          // 温度过高检查
    bool isTempLow() const;           // 温度过低检查
    bool isHumidityHigh() const;      // 湿度过高检查
    bool isHumidityLow() const;       // 湿度过低检查
    bool getIsValid() const;          // 获取数据有效性
};
```

**设计特点**:
- 缓存机制：避免频繁读取导致DHT卡住
- 数据校验：NaN检查确保数据有效性
- 阈值判断：集成环境警告逻辑

#### OLED_Display 类
```cpp
enum class EmotionState {
    HAPPY, SAD, ANGRY, SLEEPY, SURPRISED, NORMAL,
    HOT_WARNING, COLD_WARNING, HUMID_WARNING
};

class OLEDDisplay {
  private:
    U8G2_SSD1315_128X64_1_HW_I2C u8g2;
    EmotionState currentEmotion;
    
    void drawHappy();              // 绘制各种表情
    void drawSad();
    void drawAngry();
    // ... 其他表情方法
  
  public:
    void begin();                  // 初始化
    void setEmotion(EmotionState); // 设置并显示表情
    void displayTempHumidity();    // 显示温湿度
    void displayWeather();         // 显示天气
    EmotionState getEmotion() const;
};
```

**设计特点**:
- 枚举表情类型确保类型安全
- 分离绘制逻辑便于扩展
- 状态缓存避免重复绘制

#### Servo_Controller 类
```cpp
enum class ServoAction {
    SHAKE_LEFT, SHAKE_RIGHT, NOD_UP, NOD_DOWN, RESET, IDLE
};

class ServoController {
  private:
    Servo headServo, nodServo;     // 两个舵机
    uint16_t headPos, nodPos;      // 当前位置
    
    unsigned long actionStartTime; // 动作开始时间
    ServoAction currentAction;     // 当前动作
    bool isActing;                 // 动作进行中标志
  
  public:
    void begin();                  // 初始化
    void performAction(ServoAction); // 执行动作（非阻塞）
    void update();                 // 状态更新，自动复位
    bool isPerforming() const;     // 检查是否在动作中
    void stop();                   // 停止并复位
};
```

**设计特点**:
- 非阻塞设计：使用时间戳自动复位
- 单一动作队列：一次只能执行一个动作
- 自动复位机制：动作完成后自动回到中点

#### ASRPRO_Module 类
```cpp
enum class VoiceCommand {
    UNKNOWN, HAPPY, SAD, ANGRY, SLEEPY, SURPRISED,
    SHAKE, NOD, QUERY_WEATHER, QUERY_TEMP, HELP
};

class ASRPROModule {
  private:
    String inputBuffer;            // 输入缓冲区
    unsigned long lastCommandTime; // 最后指令时间
    VoiceCommand lastCommand;      // 最后指令
    
    VoiceCommand parseCommand(const String&);
  
  public:
    void begin();                  // 初始化UART2
    void update();                 // 处理串口数据
    VoiceCommand getLastCommand(); // 获取指令（自动清除）
    String getRawData() const;     // 获取原始文本
};
```

**设计特点**:
- 自动清除机制：getLastCommand()后自动清除
- 关键词匹配：支持中文和英文
- 缓冲区溢出保护

### 2. 通信层类设计

#### WiFi_Manager 类
```cpp
class WiFiManager {
  private:
    String ssid, password;                // WiFi凭证
    unsigned long lastConnectAttempt;     // 上次连接尝试时间
    uint16_t reconnectAttempts;           // 重连次数
    const uint16_t maxReconnectAttempts = 10;
  
  public:
    void begin(const char*, const char*); // 初始化WiFi
    bool connect();                       // 连接（支持指数退避）
    void update();                        // 状态检查
    bool isConnected() const;             // 获取连接状态
    String getLocalIP() const;            // 获取IP
    int8_t getSignalStrength() const;     // 获取信号强度
};
```

**设计特点**:
- 指数退避策略：避免频繁重试导致芯片复位
- 自动重连机制
- 非阻塞连接

#### MQTT_Manager 类
```cpp
class MQTTManager {
  private:
    PubSubClient mqttClient;      // MQTT客户端
    WiFiClient* wifiClient;       // WiFi连接对象
    
    unsigned long lastHeartbeat;  // 最后心跳时间
    bool isConnected;             // 连接状态
    
    static void onMessageReceived(char*, byte*, unsigned int);
  
  public:
    void begin(const char*, uint16_t);    // 初始化
    bool connect();                       // 连接到Broker
    void disconnect();                    // 断开连接
    void update();                        // 心跳和重连
    
    bool publishTempHumidity(float, float);
    bool publishWeather(const char*, float, float);
    bool publishStatus(const char*);
    bool subscribe(const char*);
    
    bool isConnectedToMQTT() const;
};
```

**设计特点**:
- 自动心跳机制（30秒）
- OneNet格式的JSON数据包
- 自动重连（5秒间隔）

#### Weather_Service 类
```cpp
struct WeatherData {
    String weather;        // 天气描述
    float temperature;     // 当前温度
    float tempMax;         // 最高温度
    float tempMin;         // 最低温度
    String updateTime;     // 更新时间
    bool isValid;          // 数据有效性
};

class WeatherService {
  private:
    WeatherData currentWeather;
    unsigned long lastUpdateTime;
    
    bool parseWeatherJSON(const char*);
  
  public:
    void begin();                  // 初始化
    bool updateWeather();          // 从API获取数据
    WeatherData getWeather() const;
    bool needsUpdate() const;      // 检查是否需要更新
};
```

**设计特点**:
- 10分钟更新间隔（避免频繁请求）
- ArduinoJson解析
- 自动超时保护（5秒）

### 3. 交互层类设计

#### State_Machine 类
```cpp
enum class SystemState {
    IDLE,              // 初始化状态
    MONITORING,        // 监测环境
    VOICE_TRIGGERED,   // 语音触发
    TEMP_WARNING,      // 温度警告
    HUMID_WARNING,     // 湿度警告
    WEATHER_DISPLAY    // 显示天气
};

class StateMachine {
  private:
    SystemState currentState, previousState;
    unsigned long stateChangeTime;
    
    // 模块引用
    OLEDDisplay* display;
    ServoController* servo;
    DHTManager* dhtManager;
    ASRPROModule* asrModule;
    
    // 状态处理函数
    void handleIdleState();
    void handleMonitoringState();
    void handleVoiceTriggeredState();
    void handleTempWarningState();
    void handleHumidWarningState();
    void handleWeatherDisplayState();
    
    // 辅助函数
    void transitionTo(SystemState);
    void processVoiceCommand(VoiceCommand);
    void checkEnvironmentalThresholds();
  
  public:
    void begin(OLEDDisplay*, ServoController*, DHTManager*, ASRPROModule*);
    void update();  // 状态机主驱动
    SystemState getCurrentState() const;
    unsigned long getStateElapsedTime() const;
};
```

**设计特点**:
- 清晰的状态转移图
- 事件驱动的状态变化
- 非阻塞的状态处理

## 模块交互序列图

### 启动序列
```
main.setup()
    │
    ├─→ setupSerialCommunication()
    ├─→ setupDisplay()         ──→ oledDisplay.begin()
    ├─→ setupSensorsAndActuators()
    │   ├─→ dhtManager.begin()
    │   ├─→ servoController.begin()  ──→ 舵机复位到90°
    │   └─→ asrModule.begin()
    ├─→ setupWiFiAndCloud()
    │   ├─→ wifiManager.begin()
    │   ├─→ mqttManager.begin()
    │   └─→ weatherService.begin()
    └─→ setupStateMachine()    ──→ fsm.begin()

fsm.currentState = IDLE (→ 3秒后自动转MONITORING)
```

### 运行主循环
```
loop()
  │
  ├─→ readSensors()
  │   └─→ dhtManager.read()  [2秒间隔]
  │
  ├─→ asrModule.update()     [读串口2]
  │
  ├─→ fsm.update()           [核心业务逻辑]
  │   ├─→ 检查语音指令
  │   │   └─→ processVoiceCommand()
  │   │       ├─→ display.setEmotion()
  │   │       └─→ servo.performAction()
  │   │
  │   ├─→ 检查环境阈值
  │   │   └─→ checkEnvironmentalThresholds()
  │   │       └─→ 触发警告表情和动作
  │   │
  │   └─→ 根据状态执行处理
  │       └─→ handleXxxState()
  │
  ├─→ servo.update()         [自动复位]
  │
  ├─→ updateWiFiConnection()
  │   └─→ wifiManager.update()  [检查连接]
  │
  ├─→ updateMQTTConnection()
  │   ├─→ mqttManager.update()  [心跳+重连]
  │   └─→ mqttClient.publish()  [定期上报]
  │
  ├─→ updateWeatherData()
  │   └─→ weatherService.updateWeather()  [10分钟更新]
  │
  └─→ delay(10)              [防止WDT]
```

### 语音指令处理流程
```
ASRPRO收到"开心"
    │
    └─→ asrModule.update()
        └─→ parseCommand("开心")
            └─→ return VoiceCommand::HAPPY

loop() 中
    └─→ fsm.update()
        └─→ voiceCmd = asrModule.getLastCommand()  [返回HAPPY]
            └─→ fsm.processVoiceCommand(HAPPY)
                ├─→ display.setEmotion(HAPPY)
                │   └─→ oledDisplay.drawHappy()
                │       └─→ 显示开心表情
                └─→ servo.performAction(NOD_UP)
                    ├─→ servoController.setNodPosition(30)
                    └─→ 等待500ms自动复位
```

## 数据流向图

```
┌─────────────┐
│ 物理传感器  │
└────┬────────┘
     │ GPIO/I2C/UART
     │
     ▼
┌─────────────────────────────────┐
│ 驱动层                          │
├─────────────────────────────────┤
│• DHT_Manager  (温湿度 + 阈值)   │
│• OLED_Display (UI表现)          │
│• Servo_Controller (动作)        │
│• ASRPRO_Module (语音指令)       │
└────┬────────────────────────────┘
     │ 数据对象 & 指令
     │
     ▼
┌─────────────────────────────────┐
│ 交互层 (State_Machine)          │
├─────────────────────────────────┤
│• 事件识别                       │
│• 状态转移                       │
│• 决策与驱动                     │
└────┬────────────────────────────┘
     │ 执行指令
     │
     ▼
┌─────────────────────────────────┐
│ 通信层                          │
├─────────────────────────────────┤
│• WiFi_Manager (网络连接)        │
│• MQTT_Manager (数据上报)        │
│• Weather_Service (数据拉取)     │
└────┬────────────────────────────┘
     │ UART/网络
     │
     ▼
┌──────────────────────┐
│ 外部系统             │
├──────────────────────┤
│• ESP8266 WiFi       │
│• OneNet 云平台      │
│• 心知天气 API       │
└──────────────────────┘
```

## 扩展性设计

### 添加新传感器
```cpp
class NewSensor {
  public:
    void begin();
    bool read();
    float getValue() const;
};

// 在main.cpp中
NewSensor newSensor;

void setup() {
    newSensor.begin();
}

void loop() {
    if (newSensor.read()) {
        // 使用数据
    }
}
```

### 添加新表情
```cpp
// 在OLED_Display.h中
enum class EmotionState {
    // ... 现有表情 ...
    CONFUSED,  // 新增
    EXCITED    // 新增
};

// 在OLED_Display.cpp中
void OLEDDisplay::drawConfused() {
    // 绘制实现
}

void OLEDDisplay::setEmotion(EmotionState emotion) {
    switch (emotion) {
        // ...
        case EmotionState::CONFUSED:
            drawConfused();
            break;
        // ...
    }
}
```

### 添加新状态
```cpp
enum class SystemState {
    // ... 现有状态 ...
    CUSTOM_STATE  // 新增
};

// 在State_Machine.cpp中
void StateMachine::handleCustomState() {
    // 状态处理逻辑
}

void StateMachine::update() {
    // ...
    case SystemState::CUSTOM_STATE:
        handleCustomState();
        break;
    // ...
}
```

## 优化建议

1. **内存优化**: 使用`PROGMEM`存储常量字符串
2. **性能优化**: 使用位操作进行标志管理
3. **功耗优化**: 实现睡眠模式降低功耗
4. **可靠性**: 添加WDT看门狗保护
