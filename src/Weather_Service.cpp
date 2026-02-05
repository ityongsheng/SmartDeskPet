#include "Weather_Service.h"
#include <WiFi.h>
#include <WiFiClient.h>

WeatherService::WeatherService() 
    : lastUpdateTime(0) {
    currentWeather.isValid = false;
    currentWeather.weather = "Unknown";
    currentWeather.temperature = 0;
    currentWeather.tempMax = 0;
    currentWeather.tempMin = 0;
}

void WeatherService::begin() {
    lastUpdateTime = millis();
}

bool WeatherService::updateWeather() {
    // 检查是否已经在更新间隔内
    if (!needsUpdate()) {
        return currentWeather.isValid;
    }
    
    Serial.println("[Weather] Fetching weather data...");
    
    WiFiClient client;
    
    // 连接到心知天气服务器
    if (!client.connect(WEATHER_API_URL, 80)) {
        Serial.println("[Weather] Failed to connect to API server");
        return false;
    }
    
    // 发送HTTP请求
    String request = String("GET ") + WEATHER_API_PATH + " HTTP/1.1\r\n";
    request += "Host: " + String(WEATHER_API_URL) + "\r\n";
    request += "Connection: close\r\n\r\n";
    
    client.print(request);
    
    // 等待响应
    String response = "";
    bool bodyStarted = false;
    unsigned long timeout = millis() + 5000;  // 5秒超时
    
    while (client.connected() || client.available()) {
        if (millis() > timeout) {
            Serial.println("[Weather] Request timeout");
            break;
        }
        
        if (client.available()) {
            String line = client.readStringUntil('\n');
            
            if (!bodyStarted) {
                if (line == "\r") {
                    bodyStarted = true;
                }
            } else {
                response += line;
            }
        }
    }
    
    client.stop();
    
    if (response.length() > 0) {
        return parseWeatherJSON(response.c_str());
    }
    
    return false;
}

bool WeatherService::parseWeatherJSON(const char* jsonStr) {
    // 使用ArduinoJson解析JSON
    // 注意：心知天气API的响应格式
    
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        Serial.print("[Weather] JSON parse error: ");
        Serial.println(error.c_str());
        return false;
    }
    
    // 检查响应状态
    if (doc["status"] != "ok") {
        Serial.println("[Weather] API returned non-ok status");
        return false;
    }
    
    // 提取数据
    try {
        JsonObject results = doc["results"][0];
        JsonObject now = results["now"];
        JsonObject last_update = results["last_update"];
        
        currentWeather.weather = String((const char*)now["text"]);
        currentWeather.temperature = now["temperature"].as<float>();
        currentWeather.tempMax = results["daily"][0]["high"].as<float>();
        currentWeather.tempMin = results["daily"][0]["low"].as<float>();
        currentWeather.updateTime = String((const char*)last_update["datetime"]);
        currentWeather.isValid = true;
        
        lastUpdateTime = millis();
        
        Serial.print("[Weather] Updated: ");
        Serial.print(currentWeather.weather);
        Serial.print(" T=");
        Serial.print(currentWeather.temperature);
        Serial.println("C");
        
        return true;
        
    } catch (...) {
        Serial.println("[Weather] Failed to extract data from JSON");
        currentWeather.isValid = false;
        return false;
    }
}
