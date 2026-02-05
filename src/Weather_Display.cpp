#include "Weather_Display.h"

WeatherDisplay::WeatherDisplay() 
    : display(nullptr), bitmapLib(nullptr), 
      currentWeatherType(WeatherType::UNKNOWN),
      lastTempMax(0), lastTempMin(0) {
}

void WeatherDisplay::begin(OLEDDisplay* disp, EmotionBitmapLibrary* bitmap) {
    display = disp;
    bitmapLib = bitmap;
}

WeatherType WeatherDisplay::parseWeatherType(const String& weatherText) {
    String lower = weatherText;
    lower.toLowerCase();
    
    if (lower.indexOf("晴") != -1 || lower.indexOf("sunny") != -1) {
        return WeatherType::SUNNY;
    }
    else if (lower.indexOf("云") != -1 || lower.indexOf("cloudy") != -1) {
        return WeatherType::CLOUDY;
    }
    else if (lower.indexOf("雨") != -1 || lower.indexOf("rain") != -1) {
        return WeatherType::RAINY;
    }
    else if (lower.indexOf("雪") != -1 || lower.indexOf("snow") != -1) {
        return WeatherType::SNOWY;
    }
    else if (lower.indexOf("暴风") != -1 || lower.indexOf("storm") != -1) {
        return WeatherType::STORMY;
    }
    
    return WeatherType::UNKNOWN;
}

void WeatherDisplay::drawDetailedWeatherUI(const char* weather, 
                                          float tempMax, float tempMin) {
    if (!display) return;
    
    display->clear();
    display->setFont(u8g2_font_ncenB12_tr);
    
    // 标题
    display->drawStr(5, 20, "Weather");
    
    // 天气文本
    display->setFont(u8g2_font_ncenB10_tr);
    display->drawStr(5, 40, weather);
    
    // 温度范围
    char tempStr[40];
    sprintf(tempStr, "High: %.0fC  Low: %.0fC", tempMax, tempMin);
    display->setFont(u8g2_font_ncenB08_tr);
    display->drawStr(5, 55, tempStr);
    
    // 天气图标（如果有）
    if (bitmapLib) {
        bitmapLib->drawWeatherIcon(weather, 80, 15);
    }
}

void WeatherDisplay::drawCompactWeatherUI(const char* weather, 
                                         float tempMax, float tempMin) {
    if (!display) return;
    
    display->clear();
    display->setFont(u8g2_font_ncenB08_tr);
    
    // 紧凑格式：天气文本 + 温度范围
    char line1[50];
    sprintf(line1, "%s", weather);
    display->drawStr(5, 20, line1);
    
    char line2[50];
    sprintf(line2, "T: %.0f-%.0fC", tempMin, tempMax);
    display->drawStr(5, 35, line2);
    
    // 实时建议
    String advice = generateWeatherAdvice(weather, (tempMax + tempMin) / 2, 50);
    display->drawStr(5, 50, advice.c_str());
}

void WeatherDisplay::displayWeatherFull(const char* weather, 
                                       float tempMax, float tempMin) {
    lastWeatherText = weather;
    lastTempMax = tempMax;
    lastTempMin = tempMin;
    currentWeatherType = parseWeatherType(weather);
    
    drawDetailedWeatherUI(weather, tempMax, tempMin);
}

void WeatherDisplay::displayWeatherCompact(const char* weather, 
                                          float tempMax, float tempMin) {
    lastWeatherText = weather;
    lastTempMax = tempMax;
    lastTempMin = tempMin;
    currentWeatherType = parseWeatherType(weather);
    
    drawCompactWeatherUI(weather, tempMax, tempMin);
}

void WeatherDisplay::displayWeatherTrend(float tempCurrent, 
                                        float tempMax, float tempMin) {
    if (!display) return;
    
    display->clear();
    display->setFont(u8g2_font_ncenB08_tr);
    
    // 温度趋势
    display->drawStr(5, 15, "Temperature Trend");
    
    char currentStr[30];
    sprintf(currentStr, "Now: %.1fC", tempCurrent);
    display->drawStr(5, 30, currentStr);
    
    // 绘制简单的温度条形图
    uint8_t maxBar = map(tempMax, 0, 50, 0, 60);
    uint8_t minBar = map(tempMin, 0, 50, 0, 60);
    uint8_t curBar = map(tempCurrent, 0, 50, 0, 60);
    
    display->drawLine(5, 50, 5 + maxBar, 50);  // 最高温
    display->drawLine(5, 58, 5 + minBar, 58);  // 最低温
    display->drawLine(5 + curBar, 45, 5 + curBar, 63);  // 当前温度标记
}

String WeatherDisplay::generateWeatherAdvice(const char* weather, 
                                            float temp, float humidity) {
    String advice = "";
    
    // 根据天气和温度给出建议
    if (temp > 30) {
        advice += "Hot! Stay cool";
    } 
    else if (temp < 10) {
        advice += "Cold! Keep warm";
    }
    else if (humidity > 70) {
        advice += "Humid, bring umbrella";
    }
    else {
        advice += "Comfortable weather";
    }
    
    return advice;
}
