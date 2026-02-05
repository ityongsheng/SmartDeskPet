#include "Remote_Control.h"

RemoteControlModule::RemoteControlModule() 
    : lastCommandTime(0), commandCount(0) {
    lastCommand.isValid = false;
}

void RemoteControlModule::begin() {
    Serial.println("[RemoteControl] Module initialized");
}

RemoteControlData RemoteControlModule::parseRemoteCommand(const char* jsonStr) {
    RemoteControlData result;
    result.isValid = false;
    result.command = RemoteCommand::INVALID;
    
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, jsonStr);
    
    if (error) {
        Serial.print("[RemoteControl] JSON parse error: ");
        Serial.println(error.c_str());
        return result;
    }
    
    // 解析指令类型
    const char* cmdType = doc["cmd"];
    if (!cmdType) {
        return result;
    }
    
    String cmd(cmdType);
    cmd.toLowerCase();
    
    if (cmd == "emotion") {
        result.command = RemoteCommand::SET_EMOTION;
        result.emotionName = String((const char*)doc["emotion"]);
        result.isValid = true;
    }
    else if (cmd == "servo") {
        result.command = RemoteCommand::MOVE_SERVO;
        result.servoAction = String((const char*)doc["action"]);
        if (doc.containsKey("angle")) {
            result.servoAngle = doc["angle"].as<uint16_t>();
        }
        result.isValid = true;
    }
    else if (cmd == "status") {
        result.command = RemoteCommand::QUERY_STATUS;
        result.isValid = true;
    }
    else if (cmd == "mode") {
        result.command = RemoteCommand::SET_MODE;
        result.mode = String((const char*)doc["mode"]);
        result.isValid = true;
    }
    else if (cmd == "calibrate") {
        result.command = RemoteCommand::CALIBRATE;
        result.isValid = true;
    }
    
    return result;
}

void RemoteControlModule::handleMQTTMessage(const char* topic, 
                                           const char* payload, 
                                           unsigned int length) {
    // 创建空终止的字符串
    char buffer[512];
    unsigned int copyLen = (length < sizeof(buffer) - 1) ? length : sizeof(buffer) - 1;
    strncpy(buffer, (const char*)payload, copyLen);
    buffer[copyLen] = '\0';
    
    Serial.print("[RemoteControl] Received: ");
    Serial.println(buffer);
    
    RemoteControlData cmd = parseRemoteCommand(buffer);
    if (cmd.isValid) {
        lastCommand = cmd;
        lastCommandTime = millis();
        commandCount++;
        
        Serial.print("[RemoteControl] Valid command received: ");
        Serial.println((int)cmd.command);
    }
}

String RemoteControlModule::generateStatusJSON(const char* emotion, 
                                              float temp, float humidity, 
                                              const char* weather) {
    DynamicJsonDocument doc(512);
    
    doc["emotion"] = emotion;
    doc["temperature"] = serialized(String(temp, 1));
    doc["humidity"] = serialized(String(humidity, 1));
    doc["weather"] = weather;
    doc["timestamp"] = millis();
    doc["commands_received"] = commandCount;
    
    String output;
    serializeJson(doc, output);
    return output;
}
