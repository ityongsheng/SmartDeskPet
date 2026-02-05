#include "MQTT_Manager.h"
#include <WiFi.h>

// 全局指针用于回调
MQTTManager* g_mqttManagerPtr = nullptr;

// 静态消息回调
void MQTTManager::onMessageReceived(char* topic, byte* payload, unsigned int length) {
    // 可以在这里处理来自OneNet的命令
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

MQTTManager::MQTTManager(WiFiClient& client) 
    : mqttClient(client), wifiClient(&client), 
      lastHeartbeat(0), isConnected(false) {
    g_mqttManagerPtr = this;
    mqttClient.setCallback(onMessageReceived);
}

void MQTTManager::begin(const char* broker, uint16_t port) {
    mqttClient.setServer(broker, port);
}

bool MQTTManager::connect() {
    if (isConnected) {
        return true;
    }
    
    // 使用 OneNet 的连接格式：username=device_id&password=api_key
    String username = MQTT_USERNAME;
    String password = MQTT_PASSWORD;
    
    Serial.println("[MQTT] Attempting to connect...");
    
    if (mqttClient.connect(MQTT_CLIENT_ID, 
                          username.c_str(), 
                          password.c_str())) {
        Serial.println("[MQTT] Connected successfully!");
        isConnected = true;
        lastHeartbeat = millis();
        
        // 订阅控制主题
        subscribe(MQTT_TOPIC_CONTROL);
        
        return true;
    } else {
        Serial.print("[MQTT] Connection failed, rc=");
        Serial.println(mqttClient.state());
        isConnected = false;
        return false;
    }
}

void MQTTManager::disconnect() {
    if (isConnected) {
        mqttClient.disconnect();
        isConnected = false;
    }
}

void MQTTManager::update() {
    // 检查连接状态
    if (!isConnected) {
        // 尝试重新连接
        if (millis() - lastHeartbeat > 5000) {  // 5秒重试一次
            connect();
            lastHeartbeat = millis();
        }
    } else {
        // 保持连接活跃
        if (!mqttClient.loop()) {
            isConnected = false;
            Serial.println("[MQTT] Connection lost, attempting to reconnect...");
        }
        
        // 定期发送心跳
        if (millis() - lastHeartbeat > MQTT_HEARTBEAT_INTERVAL) {
            lastHeartbeat = millis();
            publishStatus("online");
        }
    }
}

bool MQTTManager::publishTempHumidity(float temp, float humidity) {
    if (!isConnected) {
        return false;
    }
    
    // OneNet 数据格式
    char payload[128];
    sprintf(payload, "{\"temp\":%.2f,\"humidity\":%.2f}", temp, humidity);
    
    bool result = mqttClient.publish(MQTT_TOPIC_STATUS, payload);
    if (result) {
        Serial.println("[MQTT] Temperature & Humidity published");
    }
    return result;
}

bool MQTTManager::publishWeather(const char* weather, float tempMax, float tempMin) {
    if (!isConnected) {
        return false;
    }
    
    char payload[256];
    sprintf(payload, "{\"weather\":\"%s\",\"temp_max\":%.1f,\"temp_min\":%.1f}", 
            weather, tempMax, tempMin);
    
    bool result = mqttClient.publish(MQTT_TOPIC_STATUS, payload);
    if (result) {
        Serial.println("[MQTT] Weather data published");
    }
    return result;
}

bool MQTTManager::publishStatus(const char* status) {
    if (!isConnected) {
        return false;
    }
    
    char payload[64];
    sprintf(payload, "{\"status\":\"%s\"}", status);
    
    return mqttClient.publish(MQTT_TOPIC_STATUS, payload);
}

bool MQTTManager::subscribe(const char* topic) {
    if (!isConnected) {
        return false;
    }
    
    bool result = mqttClient.subscribe(topic);
    if (result) {
        Serial.print("[MQTT] Subscribed to: ");
        Serial.println(topic);
    }
    return result;
}
