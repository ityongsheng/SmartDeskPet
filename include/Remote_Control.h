#ifndef REMOTE_CONTROL_H
#define REMOTE_CONTROL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"

/**
 * 远程控制模块
 * 处理来自微信小程序的MQTT命令
 * 支持: 表情切换、舵机动作、参数查询等
 */

// 远程指令类型
enum class RemoteCommand {
    INVALID,              // 无效指令
    SET_EMOTION,          // 设置表情
    MOVE_SERVO,           // 舵机动作
    QUERY_STATUS,         // 查询状态
    SET_MODE,             // 设置工作模式
    CALIBRATE,            // 校准
};

// 远程指令数据结构
struct RemoteControlData {
    RemoteCommand command;
    String emotionName;           // 表情名称
    String servoAction;           // 舵机动作名称
    uint16_t servoAngle;          // 舵机角度
    String mode;                  // 工作模式
    bool isValid;
};

/**
 * 远程控制模块类
 */
class RemoteControlModule {
private:
    RemoteControlData lastCommand;
    unsigned long lastCommandTime;
    uint16_t commandCount;
    
    /**
     * 解析JSON命令
     */
    RemoteControlData parseRemoteCommand(const char* jsonStr);
    
public:
    RemoteControlModule();
    
    /**
     * 初始化模块
     */
    void begin();
    
    /**
     * 处理MQTT接收到的消息
     */
    void handleMQTTMessage(const char* topic, const char* payload, unsigned int length);
    
    /**
     * 获取最后接收的命令
     */
    RemoteControlData getLastCommand() { 
        RemoteControlData cmd = lastCommand;
        lastCommand.isValid = false;
        return cmd;
    }
    
    /**
     * 获取命令计数（用于统计）
     */
    uint16_t getCommandCount() const { return commandCount; }
    
    /**
     * 获取最后命令时间
     */
    unsigned long getLastCommandTime() const { return lastCommandTime; }
    
    /**
     * 生成状态JSON响应
     */
    String generateStatusJSON(const char* emotion, float temp, float humidity, 
                            const char* weather);
};

#endif
