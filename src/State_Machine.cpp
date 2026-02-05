#include "State_Machine.h"

StateMachine::StateMachine() 
    : currentState(SystemState::IDLE), previousState(SystemState::IDLE),
      stateChangeTime(0), lastActionTime(0),
      display(nullptr), servo(nullptr), dhtManager(nullptr), asrModule(nullptr) {
}

void StateMachine::begin(OLEDDisplay* disp, ServoController* srv,
                         DHTManager* dht, ASRPROModule* asr) {
    display = disp;
    servo = srv;
    dhtManager = dht;
    asrModule = asr;
    
    currentState = SystemState::IDLE;
    stateChangeTime = millis();
}

void StateMachine::transitionTo(SystemState newState) {
    if (newState != currentState) {
        previousState = currentState;
        currentState = newState;
        stateChangeTime = millis();
        lastActionTime = millis();
        
        Serial.print("[FSM] State transition: ");
        Serial.print((int)previousState);
        Serial.print(" -> ");
        Serial.println((int)newState);
    }
}

void StateMachine::processVoiceCommand(VoiceCommand cmd) {
    if (!servo || !display) return;
    
    Serial.print("[FSM] Processing voice command: ");
    Serial.println((int)cmd);
    
    switch (cmd) {
        case VoiceCommand::HAPPY:
            display->setEmotion(EmotionState::HAPPY);
            servo->performAction(ServoAction::NOD_UP);
            break;
            
        case VoiceCommand::SAD:
            display->setEmotion(EmotionState::SAD);
            servo->performAction(ServoAction::NOD_DOWN);
            break;
            
        case VoiceCommand::ANGRY:
            display->setEmotion(EmotionState::ANGRY);
            servo->performAction(ServoAction::SHAKE_LEFT);
            break;
            
        case VoiceCommand::SLEEPY:
            display->setEmotion(EmotionState::SLEEPY);
            servo->performAction(ServoAction::RESET);
            break;
            
        case VoiceCommand::SURPRISED:
            display->setEmotion(EmotionState::SURPRISED);
            servo->performAction(ServoAction::NOD_UP);
            break;
            
        case VoiceCommand::SHAKE:
            servo->performAction(ServoAction::SHAKE_RIGHT);
            break;
            
        case VoiceCommand::NOD:
            servo->performAction(ServoAction::NOD_UP);
            break;
            
        case VoiceCommand::QUERY_TEMP:
            // 显示温湿度信息
            if (dhtManager) {
                display->displayTempHumidity(
                    dhtManager->getTemperature(),
                    dhtManager->getHumidity()
                );
            }
            break;
            
        default:
            break;
    }
    
    transitionTo(SystemState::VOICE_TRIGGERED);
}

void StateMachine::checkEnvironmentalThresholds() {
    if (!dhtManager || !display) return;
    
    if (!dhtManager->getIsValid()) {
        Serial.println("[FSM] DHT sensor reading invalid");
        return;
    }
    
    if (dhtManager->isTempHigh()) {
        display->setEmotion(EmotionState::HOT_WARNING);
        transitionTo(SystemState::TEMP_WARNING);
    } 
    else if (dhtManager->isTempLow()) {
        display->setEmotion(EmotionState::COLD_WARNING);
        transitionTo(SystemState::TEMP_WARNING);
    }
    else if (dhtManager->isHumidityHigh() || dhtManager->isHumidityLow()) {
        display->setEmotion(EmotionState::HUMID_WARNING);
        transitionTo(SystemState::HUMID_WARNING);
    }
    else {
        // 环境正常
        if (currentState == SystemState::TEMP_WARNING || 
            currentState == SystemState::HUMID_WARNING) {
            transitionTo(SystemState::MONITORING);
            display->setEmotion(EmotionState::NORMAL);
        }
    }
}

void StateMachine::handleIdleState() {
    // 空闲状态：等待触发事件或转移到监测状态
    if (getStateElapsedTime() > 3000) {  // 3秒后自动进入监测状态
        transitionTo(SystemState::MONITORING);
    }
}

void StateMachine::handleMonitoringState() {
    // 监测状态：定期读取传感器数据，检查阈值
    checkEnvironmentalThresholds();
    
    // 定期更新显示
    if (getStateElapsedTime() % 2000 < 100) {
        if (dhtManager && display) {
            display->displayTempHumidity(
                dhtManager->getTemperature(),
                dhtManager->getHumidity()
            );
        }
    }
}

void StateMachine::handleVoiceTriggeredState() {
    // 语音触发状态：执行完动作后回到监测状态
    if (servo && !servo->isPerforming()) {
        if (getStateElapsedTime() > 2000) {  // 动作完成后等2秒
            transitionTo(SystemState::MONITORING);
        }
    }
}

void StateMachine::handleTempWarningState() {
    // 温度警告状态：等待条件恢复
    checkEnvironmentalThresholds();
    
    // 如果警告持续超过10秒，可以触发舵机动作
    if (getStateElapsedTime() > 5000 && 
        servo && !servo->isPerforming()) {
        // 执行一个警告动作（如摇头）
        servo->performAction(ServoAction::SHAKE_LEFT);
    }
}

void StateMachine::handleHumidWarningState() {
    // 湿度警告状态
    checkEnvironmentalThresholds();
    
    if (getStateElapsedTime() > 5000 && 
        servo && !servo->isPerforming()) {
        servo->performAction(ServoAction::SHAKE_RIGHT);
    }
}

void StateMachine::handleWeatherDisplayState() {
    // 天气显示状态：3秒后回到监测状态
    if (getStateElapsedTime() > 3000) {
        transitionTo(SystemState::MONITORING);
    }
}

void StateMachine::update() {
    if (!display || !servo || !dhtManager || !asrModule) {
        return;
    }
    
    // 更新舵机状态
    servo->update();
    
    // 读取语音指令
    VoiceCommand voiceCmd = asrModule->getLastCommand();
    if (voiceCmd != VoiceCommand::UNKNOWN) {
        processVoiceCommand(voiceCmd);
    }
    
    // 根据当前状态执行相应的处理逻辑
    switch (currentState) {
        case SystemState::IDLE:
            handleIdleState();
            break;
            
        case SystemState::MONITORING:
            handleMonitoringState();
            break;
            
        case SystemState::VOICE_TRIGGERED:
            handleVoiceTriggeredState();
            break;
            
        case SystemState::TEMP_WARNING:
            handleTempWarningState();
            break;
            
        case SystemState::HUMID_WARNING:
            handleHumidWarningState();
            break;
            
        case SystemState::WEATHER_DISPLAY:
            handleWeatherDisplayState();
            break;
            
        default:
            break;
    }
}
