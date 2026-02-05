#ifndef CONFIG_H
#define CONFIG_H

// ==================== 引脚定义 ====================
// DHT11 温湿度传感器
#define DHT_PIN PA1
#define DHT_TYPE DHT11

// OLED 显示屏 (I2C)
#define OLED_SDA PB7
#define OLED_SCL PB6
#define OLED_I2C_ADDR 0x3C

// 舵机
#define SERVO_HEAD_PIN PA0      // 摇头
#define SERVO_NOD_PIN PA5       // 点头

// 串口定义
#define UART_ASRPRO Serial2     // 语音识别模块 (TX=PA2, RX=PA3)
#define UART_ESP8266 Serial3    // ESP8266 (TX=PB10, RX=PB11)
#define UART_BAUD_ASRPRO 9600
#define UART_BAUD_ESP8266 115200

// ==================== 温湿度阈值 ====================
#define TEMP_HIGH_THRESHOLD 30.0f   // 温度过高警告值
#define TEMP_LOW_THRESHOLD 15.0f    // 温度过低警告值
#define HUMID_HIGH_THRESHOLD 70.0f  // 湿度过高警告值
#define HUMID_LOW_THRESHOLD 30.0f   // 湿度过低警告值

// ==================== WiFi & MQTT 配置 ====================
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"
#define MQTT_BROKER "mqtt.heclouds.com"
#define MQTT_PORT 1883
#define MQTT_USERNAME "your_device_id"
#define MQTT_PASSWORD "your_api_key"
#define MQTT_CLIENT_ID "smartdesk_device"

// MQTT 主题
#define MQTT_TOPIC_STATUS "$dp/post/your_device_id"
#define MQTT_TOPIC_CONTROL "$dp/cmd/your_device_id"

// ==================== 心知天气 API ====================
#define WEATHER_API_URL "api.seniverse.com"
#define WEATHER_API_PATH "/v3/weather/now.json?key=YOUR_API_KEY&location=auto&lang=zh-Hans"
#define WEATHER_UPDATE_INTERVAL 600000  // 10分钟更新一次

// ==================== 时间配置 ====================
#define SENSOR_READ_INTERVAL 2000       // 传感器读取间隔 2秒
#define MQTT_HEARTBEAT_INTERVAL 30000   // MQTT心跳 30秒
#define SERVO_ACTION_DURATION 500       // 舵机动作持续时间 500ms

#endif
