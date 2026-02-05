#ifndef WEATHER_DISPLAY_H
#define WEATHER_DISPLAY_H

#include <Arduino.h>
#include "config.h"
#include "OLED_Display.h"
#include "Emotion_Bitmap.h"

/**
 * 天气显示模块
 * 集成天气图标、文字、温度范围等完整信息展示
 * 支持图形化天气表示
 */

enum class WeatherType {
    SUNNY,          // 晴天
    CLOUDY,         // 阴天
    RAINY,          // 下雨
    SNOWY,          // 下雪
    STORMY,         // 暴风雨
    UNKNOWN         // 未知
};

/**
 * 天气显示管理类
 */
class WeatherDisplay {
private:
    OLEDDisplay* display;
    EmotionBitmapLibrary* bitmapLib;
    WeatherType currentWeatherType;
    String lastWeatherText;
    float lastTempMax, lastTempMin;
    
    /**
     * 字符串转天气类型
     */
    WeatherType parseWeatherType(const String& weatherText);
    
    /**
     * 绘制详细天气界面
     */
    void drawDetailedWeatherUI(const char* weather, float tempMax, float tempMin);
    
    /**
     * 绘制紧凑天气界面
     */
    void drawCompactWeatherUI(const char* weather, float tempMax, float tempMin);
    
public:
    WeatherDisplay();
    
    /**
     * 初始化模块
     */
    void begin(OLEDDisplay* disp, EmotionBitmapLibrary* bitmap);
    
    /**
     * 显示天气信息（完整版）
     */
    void displayWeatherFull(const char* weather, float tempMax, float tempMin);
    
    /**
     * 显示天气信息（紧凑版）
     */
    void displayWeatherCompact(const char* weather, float tempMax, float tempMin);
    
    /**
     * 显示天气趋势（温度变化）
     */
    void displayWeatherTrend(float tempCurrent, float tempMax, float tempMin);
    
    /**
     * 生成天气建议文本
     */
    String generateWeatherAdvice(const char* weather, float temp, float humidity);
    
    /**
     * 获取当前天气类型
     */
    WeatherType getCurrentWeatherType() const { return currentWeatherType; }
};

#endif
