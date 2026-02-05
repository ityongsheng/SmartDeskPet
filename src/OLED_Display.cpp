#include "OLED_Display.h"

OLEDDisplay::OLEDDisplay() 
    : u8g2(U8G2_R0, OLED_SCL, OLED_SDA), 
      currentEmotion(EmotionState::NORMAL) {
}

void OLEDDisplay::begin() {
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    clear();
}

void OLEDDisplay::clear() {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void OLEDDisplay::drawHappy() {
    u8g2.clearBuffer();
    // 圆脸
    u8g2.drawCircle(64, 32, 28);
    // 左眼
    u8g2.drawCircle(50, 26, 4);
    u8g2.drawDisc(50, 26, 2);
    // 右眼
    u8g2.drawCircle(78, 26, 4);
    u8g2.drawDisc(78, 26, 2);
    // 开心的嘴（弧线）
    u8g2.drawCircle(64, 35, 10);
    u8g2.drawLine(55, 35, 73, 35);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawSad() {
    u8g2.clearBuffer();
    // 圆脸
    u8g2.drawCircle(64, 32, 28);
    // 左眼
    u8g2.drawCircle(50, 26, 4);
    u8g2.drawDisc(50, 26, 2);
    // 右眼
    u8g2.drawCircle(78, 26, 4);
    u8g2.drawDisc(78, 26, 2);
    // 伤心的嘴
    u8g2.drawCircle(64, 42, 8);
    u8g2.drawLine(55, 42, 73, 42);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawAngry() {
    u8g2.clearBuffer();
    // 圆脸
    u8g2.drawCircle(64, 32, 28);
    // 左眼（生气眉毛）
    u8g2.drawLine(45, 20, 55, 22);
    u8g2.drawCircle(50, 26, 4);
    u8g2.drawDisc(50, 26, 2);
    // 右眼（生气眉毛）
    u8g2.drawLine(73, 22, 83, 20);
    u8g2.drawCircle(78, 26, 4);
    u8g2.drawDisc(78, 26, 2);
    // 生气的嘴
    u8g2.drawLine(55, 40, 73, 40);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawSleepy() {
    u8g2.clearBuffer();
    // 圆脸
    u8g2.drawCircle(64, 32, 28);
    // 闭眼（横线）
    u8g2.drawLine(45, 26, 55, 26);
    u8g2.drawLine(73, 26, 83, 26);
    // 嘴（平线）
    u8g2.drawLine(55, 40, 73, 40);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawSurprised() {
    u8g2.clearBuffer();
    // 圆脸
    u8g2.drawCircle(64, 32, 28);
    // 左眼
    u8g2.drawCircle(50, 26, 5);
    u8g2.drawDisc(50, 26, 2);
    // 右眼
    u8g2.drawCircle(78, 26, 5);
    u8g2.drawDisc(78, 26, 2);
    // 惊讶的嘴（圆形）
    u8g2.drawCircle(64, 42, 6);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawNormal() {
    u8g2.clearBuffer();
    // 圆脸
    u8g2.drawCircle(64, 32, 28);
    // 左眼
    u8g2.drawCircle(50, 26, 4);
    u8g2.drawDisc(50, 26, 2);
    // 右眼
    u8g2.drawCircle(78, 26, 4);
    u8g2.drawDisc(78, 26, 2);
    // 普通的嘴
    u8g2.drawLine(55, 40, 73, 40);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawHotWarning() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(10, 20, "TEMP HIGH!");
    u8g2.drawStr(10, 40, "Cool Down!");
    u8g2.drawStr(10, 60, "T > 30C");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawColdWarning() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(10, 20, "TEMP LOW!");
    u8g2.drawStr(10, 40, "Keep Warm!");
    u8g2.drawStr(10, 60, "T < 15C");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();
}

void OLEDDisplay::drawHumidityWarning() {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(10, 20, "HUMIDITY!");
    u8g2.drawStr(10, 40, "Abnormal RH");
    u8g2.drawStr(10, 60, "Check Env!");
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();
}

void OLEDDisplay::setEmotion(EmotionState emotion) {
    currentEmotion = emotion;
    
    switch (emotion) {
        case EmotionState::HAPPY:
            drawHappy();
            break;
        case EmotionState::SAD:
            drawSad();
            break;
        case EmotionState::ANGRY:
            drawAngry();
            break;
        case EmotionState::SLEEPY:
            drawSleepy();
            break;
        case EmotionState::SURPRISED:
            drawSurprised();
            break;
        case EmotionState::HOT_WARNING:
            drawHotWarning();
            break;
        case EmotionState::COLD_WARNING:
            drawColdWarning();
            break;
        case EmotionState::HUMID_WARNING:
            drawHumidityWarning();
            break;
        case EmotionState::NORMAL:
        default:
            drawNormal();
            break;
    }
}

void OLEDDisplay::displayTempHumidity(float temp, float humidity) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB12_tr);
    
    char tempStr[20];
    char humidStr[20];
    sprintf(tempStr, "Temp: %.1fC", temp);
    sprintf(humidStr, "Humid: %.1f%%", humidity);
    
    u8g2.drawStr(5, 25, tempStr);
    u8g2.drawStr(5, 50, humidStr);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();
}

void OLEDDisplay::displayWeather(const char* weather, float tempMax, float tempMin) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    
    char maxStr[20];
    char minStr[20];
    sprintf(maxStr, "Max: %.0fC", tempMax);
    sprintf(minStr, "Min: %.0fC", tempMin);
    
    u8g2.drawStr(10, 20, "Weather:");
    u8g2.drawStr(10, 35, weather);
    u8g2.drawStr(10, 50, maxStr);
    u8g2.drawStr(10, 60, minStr);
    u8g2.sendBuffer();
}

void OLEDDisplay::displayIP(const char* ip) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(10, 20, "IP:");
    u8g2.drawStr(10, 35, ip);
    u8g2.sendBuffer();
}

void OLEDDisplay::displayMQTTStatus(bool connected) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB10_tr);
    
    if (connected) {
        u8g2.drawStr(20, 35, "MQTT OK");
    } else {
        u8g2.drawStr(10, 35, "MQTT Failed");
    }
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.sendBuffer();
}
