#include "DHT_Manager.h"

DHTManager::DHTManager() 
    : dht(DHT_PIN, DHT_TYPE), lastTemp(0), lastHumidity(0), 
      lastReadTime(0), isValid(false) {
}

void DHTManager::begin() {
    dht.begin();
}

bool DHTManager::read() {
    // DHT11 最小读取间隔约1秒
    if (millis() - lastReadTime < 1000) {
        return isValid;
    }
    
    lastReadTime = millis();
    
    // 读取湿度
    float humidity = dht.readHumidity();
    // 读取温度（摄氏度）
    float temperature = dht.readTemperature();
    
    // 检查是否读取失败
    if (isnan(humidity) || isnan(temperature)) {
        isValid = false;
        return false;
    }
    
    lastHumidity = humidity;
    lastTemp = temperature;
    isValid = true;
    
    return true;
}
