#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

/**
 * WiFi 连接管理类
 * 处理 ESP8266 的 WiFi 连接和重连
 */
class WiFiManager {
private:
    String ssid;
    String password;
    unsigned long lastConnectAttempt;
    uint16_t reconnectAttempts;
    const uint16_t maxReconnectAttempts = 10;
    
public:
    WiFiManager();
    
    /**
     * 初始化WiFi
     */
    void begin(const char* ssid, const char* password);
    
    /**
     * 连接到WiFi
     */
    bool connect();
    
    /**
     * 检查连接状态并进行必要的重连
     * 应在主循环中定期调用
     */
    void update();
    
    /**
     * 获取连接状态
     */
    bool isConnected() const { 
        return WiFi.status() == WL_CONNECTED; 
    }
    
    /**
     * 获取本地IP地址
     */
    String getLocalIP() const { 
        return WiFi.localIP().toString(); 
    }
    
    /**
     * 获取信号强度
     */
    int8_t getSignalStrength() const { 
        return WiFi.RSSI(); 
    }
    
    /**
     * 断开WiFi连接
     */
    void disconnect();
};

#endif
