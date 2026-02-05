#include "WiFi_Manager.h"

WiFiManager::WiFiManager() 
    : ssid(""), password(""), lastConnectAttempt(0), reconnectAttempts(0) {
}

void WiFiManager::begin(const char* ssidStr, const char* passwordStr) {
    ssid = ssidStr;
    password = passwordStr;
    
    // 设置为Station模式
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
}

bool WiFiManager::connect() {
    if (isConnected()) {
        reconnectAttempts = 0;
        return true;
    }
    
    unsigned long currentTime = millis();
    
    // 避免频繁重试，采用指数退避策略
    unsigned long retryDelay = (1000 * (1 << reconnectAttempts));  // 1s, 2s, 4s, 8s...
    if (retryDelay > 60000) retryDelay = 60000;  // 最多间隔60秒
    
    if (currentTime - lastConnectAttempt < retryDelay) {
        return false;
    }
    
    lastConnectAttempt = currentTime;
    
    if (reconnectAttempts < maxReconnectAttempts) {
        Serial.print("[WiFi] Connecting to SSID: ");
        Serial.print(ssid);
        Serial.print(" (Attempt ");
        Serial.print(reconnectAttempts + 1);
        Serial.println(")");
        
        WiFi.begin(ssid.c_str(), password.c_str());
        reconnectAttempts++;
        
        return false;  // 还在连接过程中
    }
    
    return false;
}

void WiFiManager::update() {
    if (isConnected()) {
        // 已连接
        if (reconnectAttempts > 0) {
            Serial.print("[WiFi] Connected! IP: ");
            Serial.println(getLocalIP());
            Serial.print("[WiFi] Signal strength: ");
            Serial.println(getSignalStrength());
            reconnectAttempts = 0;
        }
    } else {
        // 未连接，尝试连接
        connect();
    }
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true);  // true = 关闭WiFi
}
