#ifndef DHT_MANAGER_H
#define DHT_MANAGER_H

#include <Arduino.h>
#include "DHT.h"
#include "config.h"

/**
 * DHT11 温湿度传感器管理类
 * 负责读取传感器数据并进行校验
 */
class DHTManager {
private:
    DHT dht;
    float lastTemp;
    float lastHumidity;
    unsigned long lastReadTime;
    bool isValid;
    
public:
    DHTManager();
    
    /**
     * 初始化传感器
     */
    void begin();
    
    /**
     * 读取温湿度数据
     * @return true 读取成功，false 读取失败
     */
    bool read();
    
    /**
     * 获取温度值
     * @return 温度（摄氏度）
     */
    float getTemperature() const { return lastTemp; }
    
    /**
     * 获取湿度值
     * @return 湿度（%RH）
     */
    float getHumidity() const { return lastHumidity; }
    
    /**
     * 检查温度是否超过高阈值
     */
    bool isTempHigh() const { return lastTemp > TEMP_HIGH_THRESHOLD; }
    
    /**
     * 检查温度是否低于低阈值
     */
    bool isTempLow() const { return lastTemp < TEMP_LOW_THRESHOLD; }
    
    /**
     * 检查湿度是否超过高阈值
     */
    bool isHumidityHigh() const { return lastHumidity > HUMID_HIGH_THRESHOLD; }
    
    /**
     * 检查湿度是否低于低阈值
     */
    bool isHumidityLow() const { return lastHumidity < HUMID_LOW_THRESHOLD; }
    
    /**
     * 获取最后一次读取状态
     */
    bool getIsValid() const { return isValid; }
};

#endif
