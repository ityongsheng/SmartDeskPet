#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "config.h"
#include "OLED_Display.h"
#include "Servo_Controller.h"
#include "DHT_Manager.h"
#include "ASRPRO_Module.h"

/**
 * 系统状态
 */
enum class SystemState {
    IDLE,              // 空闲状态
    MONITORING,        // 监测环境
    VOICE_TRIGGERED,   // 语音触发
    TEMP_WARNING,      // 温度警告
    HUMID_WARNING,     // 湿度警告
    WEATHER_DISPLAY,   // 显示天气
};

/**
 * 状态机类
 * 管理系统的状态转移和动作触发
 */
class StateMachine {
private:
    SystemState currentState;
    SystemState previousState;
    unsigned long stateChangeTime;
    unsigned long lastActionTime;
    
    // 引用到各个模块
    OLEDDisplay* display;
    ServoController* servo;
    DHTManager* dhtManager;
    ASRPROModule* asrModule;
    
    // 状态转移函数
    void handleIdleState();
    void handleMonitoringState();
    void handleVoiceTriggeredState();
    void handleTempWarningState();
    void handleHumidWarningState();
    void handleWeatherDisplayState();
    
    // 辅助函数
    void transitionTo(SystemState newState);
    void processVoiceCommand(VoiceCommand cmd);
    void checkEnvironmentalThresholds();
    
public:
    StateMachine();
    
    /**
     * 初始化状态机
     */
    void begin(OLEDDisplay* disp, ServoController* srv, 
               DHTManager* dht, ASRPROModule* asr);
    
    /**
     * 更新状态机
     * 应在主循环中定期调用
     */
    void update();
    
    /**
     * 获取当前状态
     */
    SystemState getCurrentState() const { return currentState; }
    
    /**
     * 获取状态持续时间
     */
    unsigned long getStateElapsedTime() const {
        return millis() - stateChangeTime;
    }
};

#endif
