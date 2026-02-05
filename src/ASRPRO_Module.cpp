#include "ASRPRO_Module.h"

ASRPROModule::ASRPROModule() 
    : inputBuffer(""), lastCommandTime(0), 
      lastCommand(VoiceCommand::UNKNOWN) {
}

void ASRPROModule::begin() {
    UART_ASRPRO.begin(UART_BAUD_ASRPRO);
}

VoiceCommand ASRPROModule::parseCommand(const String& cmd) {
    String lowerCmd = cmd;
    lowerCmd.toLowerCase();
    
    // 去除空格和特殊字符
    lowerCmd.trim();
    
    // 关键词识别
    if (lowerCmd.indexOf("happy") != -1 || lowerCmd.indexOf("开心") != -1) {
        return VoiceCommand::HAPPY;
    }
    else if (lowerCmd.indexOf("sad") != -1 || lowerCmd.indexOf("伤心") != -1 || 
             lowerCmd.indexOf("难过") != -1) {
        return VoiceCommand::SAD;
    }
    else if (lowerCmd.indexOf("angry") != -1 || lowerCmd.indexOf("生气") != -1) {
        return VoiceCommand::ANGRY;
    }
    else if (lowerCmd.indexOf("sleepy") != -1 || lowerCmd.indexOf("困") != -1 || 
             lowerCmd.indexOf("睡眠") != -1) {
        return VoiceCommand::SLEEPY;
    }
    else if (lowerCmd.indexOf("surprised") != -1 || lowerCmd.indexOf("惊讶") != -1 || 
             lowerCmd.indexOf("惊") != -1) {
        return VoiceCommand::SURPRISED;
    }
    else if (lowerCmd.indexOf("shake") != -1 || lowerCmd.indexOf("摇头") != -1) {
        return VoiceCommand::SHAKE;
    }
    else if (lowerCmd.indexOf("nod") != -1 || lowerCmd.indexOf("点头") != -1) {
        return VoiceCommand::NOD;
    }
    else if (lowerCmd.indexOf("weather") != -1 || lowerCmd.indexOf("天气") != -1) {
        return VoiceCommand::QUERY_WEATHER;
    }
    else if (lowerCmd.indexOf("temp") != -1 || lowerCmd.indexOf("温度") != -1 || 
             lowerCmd.indexOf("温") != -1) {
        return VoiceCommand::QUERY_TEMP;
    }
    else if (lowerCmd.indexOf("help") != -1 || lowerCmd.indexOf("帮助") != -1) {
        return VoiceCommand::HELP;
    }
    
    return VoiceCommand::UNKNOWN;
}

void ASRPROModule::update() {
    // 从ASRPRO读取数据
    while (UART_ASRPRO.available()) {
        char c = UART_ASRPRO.read();
        
        if (c == '\n' || c == '\r') {
            // 命令结束
            if (inputBuffer.length() > 0) {
                lastCommand = parseCommand(inputBuffer);
                lastCommandTime = millis();
                inputBuffer = "";  // 清空缓冲区
            }
        } else {
            // 累积数据
            inputBuffer += c;
            
            // 防止缓冲区溢出
            if (inputBuffer.length() > 64) {
                inputBuffer = inputBuffer.substring(inputBuffer.length() - 32);
            }
        }
    }
}
