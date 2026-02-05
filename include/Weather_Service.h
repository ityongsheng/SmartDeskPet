#ifndef WEATHER_SERVICE_H
#define WEATHER_SERVICE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"

/**
 * 天气数据结构
 */
struct WeatherData {
    String weather;        // 天气描述
    float temperature;     // 当前温度
    float tempMax;         // 最高温度
    float tempMin;         // 最低温度
    String updateTime;     // 更新时间
    bool isValid;          // 数据是否有效
};

/**
 * 天气服务类
 * 从心知天气 API 获取数据
 */
class WeatherService {
private:
    WeatherData currentWeather;
    unsigned long lastUpdateTime;
    
public:
    WeatherService();
    
    /**
     * 初始化天气服务
     */
    void begin();
    
    /**
     * 从心知天气 API 获取数据
     * 需要先调用 WiFiManager 的连接方法确保网络可用
     */
    bool updateWeather();
    
    /**
     * 获取当前天气数据
     */
    WeatherData getWeather() const { return currentWeather; }
    
    /**
     * 获取最后一次更新时间
     */
    unsigned long getLastUpdateTime() const { return lastUpdateTime; }
    
    /**
     * 检查是否需要更新（根据时间间隔）
     */
    bool needsUpdate() const {
        return (millis() - lastUpdateTime) > WEATHER_UPDATE_INTERVAL;
    }
    
    /**
     * 解析JSON天气数据
     */
    bool parseWeatherJSON(const char* jsonStr);
};

#endif
