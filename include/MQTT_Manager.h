#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <PubSubClient.h>
#include "config.h"

// 前向声明
class WiFiClient;

/**
 * MQTT 通信管理类
 * 对接 OneNet 云平台
 */
class MQTTManager {
private:
    PubSubClient mqttClient;
    WiFiClient* wifiClient;
    
    unsigned long lastHeartbeat;
    bool isConnected;
    
    /**
     * MQTT 消息回调函数（静态）
     */
    static void onMessageReceived(char* topic, byte* payload, unsigned int length);
    
public:
    MQTTManager(WiFiClient& client);
    
    /**
     * 初始化 MQTT 连接
     */
    void begin(const char* broker, uint16_t port);
    
    /**
     * 连接到 MQTT Broker
     */
    bool connect();
    
    /**
     * 断开 MQTT 连接
     */
    void disconnect();
    
    /**
     * 保持连接活跃
     * 应在主循环中定期调用
     */
    void update();
    
    /**
     * 发布温湿度数据
     */
    bool publishTempHumidity(float temp, float humidity);
    
    /**
     * 发布天气数据
     */
    bool publishWeather(const char* weather, float tempMax, float tempMin);
    
    /**
     * 发布设备状态
     */
    bool publishStatus(const char* status);
    
    /**
     * 订阅主题
     */
    bool subscribe(const char* topic);
    
    /**
     * 获取连接状态
     */
    bool isConnectedToMQTT() const { return isConnected; }
};

#endif
