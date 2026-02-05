#include "Multimodal_Feedback.h"
#include "ASRPRO_Module.h"

MultimodalFeedbackSystem::MultimodalFeedbackSystem() 
    : display(nullptr), servo(nullptr),
      sequenceStartTime(0), currentMotionIndex(0),
      isSequencePlaying(false), currentScenario(FeedbackScenario::EXCITED),
      danceSequence(nullptr), excitedSequence(nullptr), tiredSequence(nullptr) {
}

void MultimodalFeedbackSystem::begin(OLEDDisplay* disp, ServoController* srv) {
    display = disp;
    servo = srv;
}

void MultimodalFeedbackSystem::executeFeedbackScenario(
    FeedbackScenario scenario, 
    FeedbackAction actionType) {
    
    currentScenario = scenario;
    isSequencePlaying = true;
    sequenceStartTime = millis();
    currentMotionIndex = 0;
    
    Serial.print("[Feedback] Executing scenario: ");
    Serial.println((int)scenario);
    
    switch (scenario) {
        case FeedbackScenario::DANCE:
            executeCustomSequence(DANCE_MOTIONS, sizeof(DANCE_MOTIONS) / sizeof(DANCE_MOTIONS[0]));
            break;
            
        case FeedbackScenario::EXCITED:
            executeCustomSequence(EXCITED_MOTIONS, sizeof(EXCITED_MOTIONS) / sizeof(EXCITED_MOTIONS[0]));
            break;
            
        case FeedbackScenario::TIRED:
            executeCustomSequence(TIRED_MOTIONS, sizeof(TIRED_MOTIONS) / sizeof(TIRED_MOTIONS[0]));
            break;
            
        case FeedbackScenario::HOT:
            executeCustomSequence(HOT_MOTIONS, sizeof(HOT_MOTIONS) / sizeof(HOT_MOTIONS[0]));
            break;
            
        case FeedbackScenario::COLD:
            // 寒冷反馈：哆嗦
            applyEmotionFeedback(EmotionState::COLD_WARNING);
            servo->performAction(ServoAction::SHAKE_LEFT);
            break;
            
        case FeedbackScenario::CONFUSED:
            // 困惑反馈
            applyEmotionFeedback(EmotionState::CONFUSED);
            servo->performAction(ServoAction::TILT_LEFT);
            break;
            
        case FeedbackScenario::CELEBRATE:
            // 庆祝反馈
            applyEmotionFeedback(EmotionState::HAPPY);
            servo->performAction(ServoAction::NOD_UP);
            break;
            
        case FeedbackScenario::ALERT:
            // 警告反馈
            applyEmotionFeedback(EmotionState::ALERT);
            servo->performAction(ServoAction::SHAKE_LEFT);
            break;
    }
}

void MultimodalFeedbackSystem::executeCustomSequence(
    const FeedbackMotion* motions, 
    uint8_t count) {
    
    if (motions == nullptr || count == 0) return;
    
    isSequencePlaying = true;
    sequenceStartTime = millis();
    currentMotionIndex = 0;
    
    Serial.print("[Feedback] Starting custom sequence with ");
    Serial.print(count);
    Serial.println(" motions");
}

void MultimodalFeedbackSystem::update() {
    if (!isSequencePlaying || display == nullptr || servo == nullptr) {
        return;
    }
    
    // 这里添加实际的序列播放逻辑
    // 根据时间和当前动作索引来控制舵机和表情显示
    // 这是一个简化版本，实际应该处理多个动作的时序
    
    unsigned long elapsedTime = millis() - sequenceStartTime;
    
    // 演示：更新当前动作
    if (currentMotionIndex < 3) {  // 假设最多3个动作
        // 根据时间推进到下一个动作
        if (elapsedTime > 500) {  // 每500ms切换一个动作
            currentMotionIndex++;
            sequenceStartTime = millis();
            
            if (currentMotionIndex >= 3) {
                isSequencePlaying = false;
            }
        }
    }
}

void MultimodalFeedbackSystem::stopSequence() {
    isSequencePlaying = false;
    currentMotionIndex = 0;
    if (servo != nullptr) {
        servo->performAction(ServoAction::RESET);
    }
    Serial.println("[Feedback] Sequence stopped");
}

String MultimodalFeedbackSystem::generateThermalFeedback(float temp, float humidity) {
    String feedback = "";
    
    if (temp > 32.0) {
        feedback = "Too hot! Stay cool!";
        executeFeedbackScenario(FeedbackScenario::HOT);
    } else if (temp < 10.0) {
        feedback = "So cold! Keep warm!";
        executeFeedbackScenario(FeedbackScenario::COLD);
    } else if (temp > 28.0) {
        feedback = "Feeling warm today";
    } else if (temp < 15.0) {
        feedback = "It's chilly outside";
    } else {
        feedback = "Temperature is perfect";
    }
    
    // 湿度反馈
    if (humidity > 70.0) {
        feedback += " Humidity is high";
    } else if (humidity < 30.0) {
        feedback += " Air is dry";
    }
    
    return feedback;
}

void MultimodalFeedbackSystem::applyEmotionFeedback(EmotionState emotion) {
    if (display == nullptr) return;
    
    // 显示对应的表情
    // 这里调用OLED_Display的表情显示方法
    
    switch (emotion) {
        case EmotionState::HAPPY:
            Serial.println("[Feedback] Showing HAPPY emotion");
            // display->drawEmotion(HAPPY_BITMAP);
            break;
            
        case EmotionState::SAD:
            Serial.println("[Feedback] Showing SAD emotion");
            // display->drawEmotion(SAD_BITMAP);
            break;
            
        case EmotionState::ANGRY:
            Serial.println("[Feedback] Showing ANGRY emotion");
            // display->drawEmotion(ANGRY_BITMAP);
            break;
            
        case EmotionState::CONFUSED:
            Serial.println("[Feedback] Showing CONFUSED emotion");
            // display->drawEmotion(CONFUSED_BITMAP);
            break;
            
        case EmotionState::HOT_WARNING:
            Serial.println("[Feedback] Showing HOT emotion with sweat");
            // display->drawEmotion(HOT_SWEAT_BITMAP);
            break;
            
        case EmotionState::COLD_WARNING:
            Serial.println("[Feedback] Showing COLD emotion with shiver");
            // display->drawEmotion(COLD_SHIVER_BITMAP);
            break;
            
        case EmotionState::SLEEPY:
            Serial.println("[Feedback] Showing SLEEPY emotion");
            // display->drawEmotion(SLEEPY_BITMAP);
            break;
            
        case EmotionState::ALERT:
            Serial.println("[Feedback] Showing ALERT emotion");
            // display->drawEmotion(ALERT_BITMAP);
            break;
    }
}

/**
 * 多模态反馈系统集成示例
 * 在主程序中的使用方式：
 * 
 * // 创建全局实例
 * MultimodalFeedbackSystem multimodalFeedback;
 * 
 * // 在setup中初始化
 * multimodalFeedback.begin(&oledDisplay, &servoController);
 * 
 * // 在loop中调用update以支持非阻塞反馈
 * multimodalFeedback.update();
 * 
 * // 响应语音命令"跳个舞"
 * if (voiceCommand == VOICE_DANCE) {
 *     multimodalFeedback.executeFeedbackScenario(
 *         FeedbackScenario::DANCE,
 *         FeedbackAction::FULL_FEEDBACK
 *     );
 * }
 * 
 * // 响应温度阈值
 * if (temperature > 32.0) {
 *     String feedback = multimodalFeedback.generateThermalFeedback(temperature, humidity);
 *     asrproModule.speak(feedback.c_str());
 *     multimodalFeedback.applyEmotionFeedback(EmotionState::HOT_WARNING);
 * }
 */
