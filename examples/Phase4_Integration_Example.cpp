/**
 * 阶段4完整集成示例 (Phase 4 Integration Example)
 * 
 * SmartDesk 多模态反馈系统 - 4个开发阶段的完整实现
 * 
 * 阶段说明：
 * Phase 1: 本地驱动与交互 (Local Driver & Interaction)
 *          - DHT温湿度读取
 *          - OLED屏幕显示
 *          - 语音识别触发
 *          - 表情库支持
 * 
 * Phase 2: 端-云链路 (Edge-Cloud Link)
 *          - ESP8266 WiFi连接
 *          - OneNet MQTT通信
 *          - 远程小程序控制
 *          - JSON命令解析
 * 
 * Phase 3: 在线服务 (Online Services)
 *          - 心知天气API集成
 *          - 天气数据解析
 *          - 天气图标显示
 *          - 趋势可视化
 * 
 * Phase 4: 多模态反馈 (Multimodal Feedback)
 *          - 舵机+屏幕+语音协调
 *          - 表情动画序列
 *          - 温度阈值触发
 *          - 远程指令响应
 */

#include <Arduino.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <ArduinoJson.h>

// ==================== 模块头文件 ====================

#include "config.h"
#include "DHT_Manager.h"
#include "OLED_Display.h"
#include "Servo_Controller.h"
#include "ASRPRO_Module.h"
#include "WiFi_Manager.h"
#include "MQTT_Manager.h"
#include "Weather_Service.h"
#include "State_Machine.h"
#include "Multimodal_Feedback.h"
#include "Emotion_Bitmap_Library.h"

// ==================== 全局对象实例 ====================

// Phase 1: 本地驱动
DHT_Manager dhtManager;
OLEDDisplay oledDisplay;
ServoController servoController;
ASRPROModule asrproModule;

// Phase 2: 云连接
WiFiManager wifiManager;
MQTTManager mqttManager;

// Phase 3: 在线服务
WeatherService weatherService;

// Phase 4: 多模态反馈系统
MultimodalFeedbackSystem multimodalFeedback;
EmotionBitmapLibrary emotionLibrary;

// 状态管理
StateMachine stateMachine;

// ==================== 回调函数 ====================

/**
 * MQTT消息回调
 * 处理远程控制命令
 */
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("[MQTT] Message from ");
    Serial.print(topic);
    Serial.print(": ");
    
    // 将payload转换为字符串
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    
    // 解析JSON命令
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, message);
    
    if (!error) {
        const char* cmd = doc["cmd"];
        
        if (strcmp(cmd, "emotion") == 0) {
            // 远程设置表情
            const char* emotion = doc["emotion"];
            Serial.print("[Remote] Setting emotion to: ");
            Serial.println(emotion);
            
            if (strcmp(emotion, "happy") == 0) {
                multimodalFeedback.executeFeedbackScenario(
                    FeedbackScenario::HAPPY,
                    FeedbackAction::FULL_FEEDBACK
                );
            } else if (strcmp(emotion, "dance") == 0) {
                // 响应"跳个舞"命令
                multimodalFeedback.executeFeedbackScenario(
                    FeedbackScenario::DANCE,
                    FeedbackAction::FULL_FEEDBACK
                );
                asrproModule.speak("Dancing!");
            }
        } else if (strcmp(cmd, "servo") == 0) {
            // 远程控制舵机
            const char* action = doc["action"];
            Serial.print("[Remote] Servo action: ");
            Serial.println(action);
            
            if (strcmp(action, "shake_left") == 0) {
                servoController.performAction(ServoAction::SHAKE_LEFT);
            } else if (strcmp(action, "shake_right") == 0) {
                servoController.performAction(ServoAction::SHAKE_RIGHT);
            }
        }
    }
}

// ==================== 环境监控 ====================

/**
 * 检查温度阈值并触发多模态反馈
 * 
 * 逻辑示例：
 * - 温度 > 32°C → 显示流汗表情 + 语音播报"太热啦"
 * - 温度 < 10°C → 显示打抖表情 + 语音播报"太冷啦"
 */
void checkTemperatureThreshold(float temperature, float humidity) {
    static unsigned long lastAlertTime = 0;
    unsigned long currentTime = millis();
    
    // 防止频繁触发警告（每30秒最多一次）
    if (currentTime - lastAlertTime < 30000) {
        return;
    }
    
    if (temperature > 32.0) {
        // 高温警告
        Serial.println("[Alert] Temperature > 32°C!");
        
        // 多模态反馈
        multimodalFeedback.executeFeedbackScenario(
            FeedbackScenario::HOT,
            FeedbackAction::FULL_FEEDBACK
        );
        
        // 显示热表情
        emotionLibrary.drawEmotion(EmotionState::HOT_WARNING, 48, 24);
        
        // 语音播报
        String feedback = multimodalFeedback.generateThermalFeedback(temperature, humidity);
        asrproModule.speak(feedback.c_str());
        
        lastAlertTime = currentTime;
        
    } else if (temperature < 10.0) {
        // 低温警告
        Serial.println("[Alert] Temperature < 10°C!");
        
        multimodalFeedback.executeFeedbackScenario(
            FeedbackScenario::COLD,
            FeedbackAction::FULL_FEEDBACK
        );
        
        emotionLibrary.drawEmotion(EmotionState::COLD_WARNING, 48, 24);
        asrproModule.speak("Too cold! Keep warm!");
        
        lastAlertTime = currentTime;
    }
}

// ==================== 语音命令处理 ====================

/**
 * 语音识别回调
 * 处理本地语音命令
 */
void handleVoiceCommand(VoiceCommandType command) {
    Serial.print("[Voice] Command recognized: ");
    Serial.println((int)command);
    
    switch (command) {
        case VoiceCommandType::DANCE:
            Serial.println("→ Execute DANCE feedback");
            multimodalFeedback.executeFeedbackScenario(
                FeedbackScenario::DANCE,
                FeedbackAction::FULL_FEEDBACK
            );
            asrproModule.speak("Let's dance!");
            break;
            
        case VoiceCommandType::HAPPY:
            Serial.println("→ Show HAPPY emotion");
            multimodalFeedback.applyEmotionFeedback(EmotionState::HAPPY);
            emotionLibrary.drawEmotion(EmotionState::HAPPY, 48, 24);
            asrproModule.speak("I'm happy!");
            break;
            
        case VoiceCommandType::SAD:
            Serial.println("→ Show SAD emotion");
            multimodalFeedback.applyEmotionFeedback(EmotionState::SAD);
            emotionLibrary.drawEmotion(EmotionState::SAD, 48, 24);
            asrproModule.speak("I'm sad...");
            break;
            
        case VoiceCommandType::TURN_LEFT:
            Serial.println("→ Turn LEFT");
            servoController.performAction(ServoAction::SHAKE_LEFT);
            break;
            
        case VoiceCommandType::TURN_RIGHT:
            Serial.println("→ Turn RIGHT");
            servoController.performAction(ServoAction::SHAKE_RIGHT);
            break;
            
        case VoiceCommandType::NOD:
            Serial.println("→ NOD");
            servoController.performAction(ServoAction::NOD_UP);
            break;
            
        case VoiceCommandType::UNKNOWN:
        default:
            Serial.println("→ Unknown command");
            multimodalFeedback.applyEmotionFeedback(EmotionState::CONFUSED);
            emotionLibrary.drawEmotion(EmotionState::CONFUSED, 48, 24);
            break;
    }
}

// ==================== SETUP 初始化 ====================

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n[Setup] SmartDesk Multimodal Feedback System Starting...");
    
    // ========== Phase 1: 本地驱动初始化 ==========
    Serial.println("[Setup] Initializing Phase 1 (Local Drivers)...");
    
    // DHT温湿度传感器
    dhtManager.begin(DHT_PIN);
    Serial.println("  ✓ DHT Manager initialized");
    
    // OLED显示屏
    oledDisplay.begin();
    oledDisplay.displayMessage("SmartDesk", "Starting...");
    Serial.println("  ✓ OLED Display initialized");
    
    // 舵机控制
    servoController.begin(SERVO_PIN);
    Serial.println("  ✓ Servo Controller initialized");
    
    // 语音识别模块
    asrproModule.begin(ASRPRO_RX_PIN, ASRPRO_TX_PIN);
    Serial.println("  ✓ ASRPRO Module initialized");
    
    // 表情库
    emotionLibrary.begin(&oledDisplay.getU8G2Instance());
    emotionLibrary.drawEmotion(EmotionState::HAPPY, 48, 24);
    Serial.println("  ✓ Emotion Bitmap Library initialized");
    
    // ========== Phase 2: 云连接初始化 ==========
    Serial.println("[Setup] Initializing Phase 2 (Cloud Connection)...");
    
    // WiFi连接
    wifiManager.begin(WIFI_SSID, WIFI_PASSWORD);
    while (!wifiManager.isConnected()) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n  ✓ WiFi connected");
    
    // MQTT连接
    mqttManager.begin(MQTT_BROKER, MQTT_PORT, mqtt_callback);
    mqttManager.subscribe(MQTT_CONTROL_TOPIC);
    Serial.println("  ✓ MQTT Manager initialized");
    
    // ========== Phase 3: 在线服务初始化 ==========
    Serial.println("[Setup] Initializing Phase 3 (Online Services)...");
    
    weatherService.begin(&wifiManager);
    Serial.println("  ✓ Weather Service initialized");
    
    // ========== Phase 4: 多模态反馈系统 ==========
    Serial.println("[Setup] Initializing Phase 4 (Multimodal Feedback)...");
    
    multimodalFeedback.begin(&oledDisplay, &servoController);
    Serial.println("  ✓ Multimodal Feedback System initialized");
    
    // 状态机初始化
    stateMachine.begin();
    Serial.println("  ✓ State Machine initialized");
    
    // 显示启动完成
    oledDisplay.displayMessage("SmartDesk", "Ready!");
    asrproModule.speak("SmartDesk is ready!");
    
    Serial.println("[Setup] All systems initialized successfully!");
    Serial.println("========================================\n");
}

// ==================== LOOP 主循环 ====================

void loop() {
    // 更新所有模块
    
    // Phase 1: 本地驱动更新
    dhtManager.update();
    oledDisplay.update();
    asrproModule.update();
    
    // 检查语音命令
    if (asrproModule.hasNewCommand()) {
        handleVoiceCommand(asrproModule.getCommand());
    }
    
    // Phase 2: 云连接更新
    if (!wifiManager.isConnected()) {
        wifiManager.reconnect();
    }
    
    mqttManager.loop();
    
    // Phase 3: 在线服务更新
    weatherService.update();
    
    // Phase 4: 多模态反馈更新
    multimodalFeedback.update();
    
    // ==================== 主要逻辑 ====================
    
    // 每5秒更新一次温湿度和天气显示
    static unsigned long lastUpdateTime = 0;
    if (millis() - lastUpdateTime > 5000) {
        lastUpdateTime = millis();
        
        // 读取温湿度
        float temperature = dhtManager.getTemperature();
        float humidity = dhtManager.getHumidity();
        
        // 显示温湿度
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Temp: %.1f°C Hum: %.1f%%", temperature, humidity);
        oledDisplay.displayMessage("Status", buffer);
        
        // 检查温度阈值
        checkTemperatureThreshold(temperature, humidity);
        
        // 发布数据到MQTT
        String payload = "{\"temp\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
        mqttManager.publish(MQTT_PUBLISH_TOPIC, payload.c_str());
    }
    
    // 每10秒更新一次天气
    static unsigned long lastWeatherUpdate = 0;
    if (millis() - lastWeatherUpdate > 10000) {
        lastWeatherUpdate = millis();
        
        weatherService.fetchWeather();
        String weatherText = weatherService.getWeatherDescription();
        Serial.print("[Weather] ");
        Serial.println(weatherText);
    }
    
    delay(100);
}

/**
 * ==================== 阶段4完整功能演示 ====================
 * 
 * 核心逻辑流程：
 * 
 * 1. 语音触发反馈
 *    语音输入: "跳个舞"
 *    ↓
 *    handleVoiceCommand(DANCE)
 *    ↓
 *    multimodalFeedback.executeFeedbackScenario(DANCE, FULL_FEEDBACK)
 *    ↓
 *    舵机执行摆动动作 + OLED显示心形 + 语音播报"Dancing!"
 * 
 * 2. 温度阈值触发反馈
 *    温度读取: 35°C
 *    ↓
 *    checkTemperatureThreshold(35.0)
 *    ↓
 *    multimodalFeedback.executeFeedbackScenario(HOT, FULL_FEEDBACK)
 *    ↓
 *    表情显示流汗脸 + 舵机摇晃 + 语音播报"太热啦!"
 * 
 * 3. 远程控制反馈
 *    MQTT消息: {"cmd":"emotion","emotion":"dance"}
 *    ↓
 *    mqtt_callback() 解析JSON
 *    ↓
 *    multimodalFeedback.executeFeedbackScenario(DANCE, FULL_FEEDBACK)
 *    ↓
 *    完整的多模态反馈序列执行
 * 
 * ==================== 表情库支持的表情 ====================
 * 
 * - HAPPY: 大笑脸 (快乐)
 * - SAD: 哭泣脸 (伤心)
 * - ANGRY: 瞪眼脸 (生气)
 * - CONFUSED: 歪头脸 (困惑)
 * - HOT_WARNING: 流汗脸 (热)
 * - COLD_WARNING: 发抖脸 (冷)
 * - SLEEPY: 闭眼脸 (困倦)
 * - ALERT: 惊讶脸 (警告)
 * 
 * ==================== 舵机动作支持 ====================
 * 
 * - RESET: 复位到中间
 * - SHAKE_LEFT: 左摇
 * - SHAKE_RIGHT: 右摇
 * - NOD_UP: 点头向上
 * - NOD_DOWN: 点头向下
 * - TILT_LEFT: 向左倾斜
 * - TILT_RIGHT: 向右倾斜
 * 
 * ==================== MQTT远程控制协议 ====================
 * 
 * 命令格式:
 * Topic: /device/control
 * 
 * 1. 设置表情
 *    {"cmd":"emotion","emotion":"happy"}
 * 
 * 2. 舵机控制
 *    {"cmd":"servo","action":"shake_left","angle":45}
 * 
 * 3. 多模态反馈
 *    {"cmd":"feedback","scenario":"dance","action":"full"}
 * 
 * 响应格式:
 * Topic: /device/status
 * {"emotion":"happy","temp":25.5,"humidity":45.2,"command_count":42}
 * 
 * ==================== 性能优化注意事项 ====================
 * 
 * 1. 非阻塞设计
 *    - multimodalFeedback.update() 在每个loop循环调用
 *    - 使用时间戳而不是delay()
 *    - 序列播放在后台进行
 * 
 * 2. 内存优化
 *    - 表情位图存储在PROGMEM（闪存）
 *    - 动态分配的JSON文档使用DynamicJsonDocument
 *    - 字符缓冲区大小固定且合理
 * 
 * 3. 功耗管理
 *    - OLED显示可以配置刷新频率
 *    - MQTT连接保持活动
 *    - WiFi自动重连机制
 */
