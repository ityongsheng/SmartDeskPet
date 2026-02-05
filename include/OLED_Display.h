#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"

/**
 * 表情枚举定义
 */
enum class EmotionState {
    HAPPY,        // 开心😊
    SAD,          // 伤心😢
    ANGRY,        // 生气😠
    SLEEPY,       // 困倦😴
    SURPRISED,    // 惊讶😮
    NORMAL,       // 正常😐
    HOT_WARNING,  // 热警告🔥
    COLD_WARNING, // 冷警告❄️
    HUMID_WARNING // 湿度警告💧
};

/**
 * OLED 显示屏管理类
 * 使用 U8g2 库驱动 SSD1315
 */
class OLEDDisplay {
private:
    U8G2_SSD1315_128X64_1_HW_I2C u8g2;
    EmotionState currentEmotion;
    
    /**
     * 绘制开心表情
     */
    void drawHappy();
    
    /**
     * 绘制伤心表情
     */
    void drawSad();
    
    /**
     * 绘制生气表情
     */
    void drawAngry();
    
    /**
     * 绘制困倦表情
     */
    void drawSleepy();
    
    /**
     * 绘制惊讶表情
     */
    void drawSurprised();
    
    /**
     * 绘制正常表情
     */
    void drawNormal();
    
    /**
     * 绘制温度过高警告
     */
    void drawHotWarning();
    
    /**
     * 绘制温度过低警告
     */
    void drawColdWarning();
    
    /**
     * 绘制湿度警告
     */
    void drawHumidityWarning();
    
    /**
     * 显示文字信息（如温湿度、天气等）
     */
    void drawTextInfo(const char* text);
    
public:
    OLEDDisplay();
    
    /**
     * 初始化显示屏
     */
    void begin();
    
    /**
     * 清屏
     */
    void clear();
    
    /**
     * 设置表情状态并显示
     */
    void setEmotion(EmotionState emotion);
    
    /**
     * 获取当前表情
     */
    EmotionState getEmotion() const { return currentEmotion; }
    
    /**
     * 显示温湿度信息
     */
    void displayTempHumidity(float temp, float humidity);
    
    /**
     * 显示天气信息
     */
    void displayWeather(const char* weather, float tempMax, float tempMin);
    
    /**
     * 显示IP地址
     */
    void displayIP(const char* ip);
    
    /**
     * 显示MQTT连接状态
     */
    void displayMQTTStatus(bool connected);
};

#endif
