#include <Arduino.h>
#include "config.h"
#include "DHT_Manager.h"
#include "OLED_Display.h"
#include "Servo_Controller.h"
#include "ASRPRO_Module.h"
#include "WiFi_Manager.h"
#include "MQTT_Manager.h"
#include "Weather_Service.h"
#include "State_Machine.h"

// ==================== 全局对象 ====================
DHTManager dhtManager;
OLEDDisplay oledDisplay;
ServoController servoController;
ASRPROModule asrModule;
WiFiManager wifiManager;
WeatherService weatherService;
StateMachine fsm;

// WiFi客户端用于MQTT
WiFiClient wifiClient;
MQTTManager mqttManager(wifiClient);

// ==================== 时间管理 ====================
unsigned long lastSensorRead = 0;
unsigned long lastMQTTPublish = 0;
unsigned long lastWeatherUpdate = 0;

// ==================== 初始化函数 ====================
void setupSerialCommunication() {
    Serial.begin(115200);
    delay(500);
    
    Serial.println("\n\n");
    Serial.println("================================");
    Serial.println("Smart Desk Pet System Starting!");
    Serial.println("STM32F407VET6 + Arduino Framework");
    Serial.println("================================\n");
    
    // 初始化各个UART接口
    UART_ASRPRO.begin(UART_BAUD_ASRPRO);
    UART_ESP8266.begin(UART_BAUD_ESP8266);
    
    delay(500);
    Serial.println("[Init] UART initialized");
}

void setupDisplay() {
    oledDisplay.begin();
    oledDisplay.clear();
    oledDisplay.setEmotion(EmotionState::NORMAL);
    Serial.println("[Init] OLED Display initialized");
}

void setupSensorsAndActuators() {
    // 初始化DHT11
    dhtManager.begin();
    
    // 初始化舵机
    servoController.begin();
    
    // 初始化语音模块
    asrModule.begin();
    
    Serial.println("[Init] Sensors and actuators initialized");
}

void setupWiFiAndCloud() {
    Serial.println("[Init] Starting WiFi...");
    
    // 初始化WiFi
    wifiManager.begin(WIFI_SSID, WIFI_PASSWORD);
    wifiManager.connect();
    
    // 初始化MQTT
    mqttManager.begin(MQTT_BROKER, MQTT_PORT);
    
    // 初始化天气服务
    weatherService.begin();
    
    Serial.println("[Init] WiFi and Cloud services initialized");
}

void setupStateMachine() {
    fsm.begin(&oledDisplay, &servoController, &dhtManager, &asrModule);
    Serial.println("[Init] State Machine initialized");
}

// ==================== 主设置函数 ====================
void setup() {
    setupSerialCommunication();
    delay(500);
    
    setupDisplay();
    delay(500);
    
    setupSensorsAndActuators();
    delay(500);
    
    setupWiFiAndCloud();
    delay(500);
    
    setupStateMachine();
    delay(500);
    
    Serial.println("[Init] All systems ready!");
    oledDisplay.displayIP("Connecting...");
}

// ==================== 传感器数据读取 ====================
void readSensors() {
    if (millis() - lastSensorRead < SENSOR_READ_INTERVAL) {
        return;
    }
    
    lastSensorRead = millis();
    
    // 读取DHT11
    if (dhtManager.read()) {
        Serial.print("[Sensor] T=");
        Serial.print(dhtManager.getTemperature());
        Serial.print("C, H=");
        Serial.print(dhtManager.getHumidity());
        Serial.println("%");
    } else {
        Serial.println("[Sensor] DHT11 read failed!");
    }
}

// ==================== 通信与数据上传 ====================
void updateWiFiConnection() {
    wifiManager.update();
    
    if (wifiManager.isConnected()) {
        oledDisplay.displayIP(wifiManager.getLocalIP().c_str());
    }
}

void updateMQTTConnection() {
    mqttManager.update();
    
    // 定期发布数据
    if (millis() - lastMQTTPublish > MQTT_HEARTBEAT_INTERVAL) {
        lastMQTTPublish = millis();
        
        if (mqttManager.isConnectedToMQTT() && dhtManager.getIsValid()) {
            mqttManager.publishTempHumidity(
                dhtManager.getTemperature(),
                dhtManager.getHumidity()
            );
        }
    }
}

void updateWeatherData() {
    if (weatherService.needsUpdate() && wifiManager.isConnected()) {
        if (weatherService.updateWeather()) {
            WeatherData weather = weatherService.getWeather();
            Serial.print("[Weather] ");
            Serial.println(weather.weather);
            
            // 发布到MQTT
            if (mqttManager.isConnectedToMQTT()) {
                mqttManager.publishWeather(
                    weather.weather.c_str(),
                    weather.tempMax,
                    weather.tempMin
                );
            }
        }
    }
}

// ==================== 主循环函数 ====================
void loop() {
    // 1. 读取传感器数据
    readSensors();
    
    // 2. 处理语音指令
    asrModule.update();
    
    // 3. 更新状态机
    fsm.update();
    
    // 4. 更新舵机状态
    servoController.update();
    
    // 5. 处理WiFi连接
    updateWiFiConnection();
    
    // 6. 处理MQTT连接和数据上传
    updateMQTTConnection();
    
    // 7. 更新天气数据
    updateWeatherData();
    
    // 防止看门狗超时
    delay(10);
}