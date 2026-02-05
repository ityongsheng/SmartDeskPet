#ifndef ASRPRO_MODULE_H
#define ASRPRO_MODULE_H

#include <Arduino.h>
#include "config.h"

/**
 * 语音识别指令类型
 */
enum class VoiceCommand {
    UNKNOWN,      // 未知命令
    HAPPY,        // "开心"
    SAD,          // "伤心"
    ANGRY,        // "生气"
    SLEEPY,       // "困倦"
    SURPRISED,    // "惊讶"
    SHAKE,        // "摇头"
    NOD,          // "点头"
    QUERY_WEATHER,// "天气"
    QUERY_TEMP,   // "温度"
    HELP          // "帮助"
};

/**
 * ASRPRO 语音识别模块管理类
 */
class ASRPROModule {
private:
    String inputBuffer;
    unsigned long lastCommandTime;
    VoiceCommand lastCommand;
    
    /**
     * 解析语音指令
     */
    VoiceCommand parseCommand(const String& cmd);
    
public:
    ASRPROModule();
    
    /**
     * 初始化语音模块
     */
    void begin();
    
    /**
     * 处理语音数据
     * 应在主循环中定期调用
     */
    void update();
    
    /**
     * 获取最后一条指令
     */
    VoiceCommand getLastCommand() { 
        VoiceCommand cmd = lastCommand;
        lastCommand = VoiceCommand::UNKNOWN;  // 读取后清除
        return cmd;
    }
    
    /**
     * 获取原始文本数据
     */
    String getRawData() const { return inputBuffer; }
    
    /**
     * 清空输入缓冲区
     */
    void clearBuffer() { inputBuffer = ""; }
};

#endif
