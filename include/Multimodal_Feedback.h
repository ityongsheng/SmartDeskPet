#ifndef MULTIMODAL_FEEDBACK_H
#define MULTIMODAL_FEEDBACK_H

#include <Arduino.h>
#include "config.h"
#include "OLED_Display.h"
#include "Servo_Controller.h"

/**
 * 多模态反馈系统
 * 实现表情+动作+语音的完整反馈链
 * 支持: 视觉(OLED) + 运动(舵机) + 声音(ASRPRO播报)
 */

// 反馈动作类型
enum class FeedbackAction {
    VISUAL_ONLY,     // 仅视觉反馈
    MOTION_ONLY,     // 仅运动反馈
    VISUAL_MOTION,   // 视觉+运动
    FULL_FEEDBACK    // 完整反馈（视觉+运动+声音）
};

// 反馈场景
enum class FeedbackScenario {
    DANCE,           // 跳舞
    EXCITED,         // 兴奋
    TIRED,           // 疲劳
    HOT,             // 太热
    COLD,            // 太冷
    CONFUSED,        // 困惑
    CELEBRATE,       // 庆祝
    ALERT            // 警告
};

/**
 * 单个反馈动作
 */
struct FeedbackMotion {
    ServoAction servoAction;      // 舵机动作
    EmotionState emotion;         // 表情
    uint16_t duration;            // 持续时间（毫秒）
    const char* voiceText;        // 语音文本
    bool hasVoice;                // 是否包含语音
};

/**
 * 多模态反馈序列
 */
struct FeedbackSequence {
    const FeedbackMotion* motions;
    uint8_t motionCount;
    uint16_t totalDuration;
};

/**
 * 多模态反馈管理类
 */
class MultimodalFeedbackSystem {
private:
    OLEDDisplay* display;
    ServoController* servo;
    
    unsigned long sequenceStartTime;
    uint8_t currentMotionIndex;
    bool isSequencePlaying;
    FeedbackScenario currentScenario;
    
    // 反馈场景库
    const FeedbackSequence* danceSequence;
    const FeedbackSequence* excitedSequence;
    const FeedbackSequence* tiredSequence;
    
    /**
     * 创建舞蹈序列
     */
    void createDanceSequence();
    
    /**
     * 创建兴奋序列
     */
    void createExcitedSequence();
    
    /**
     * 创建疲劳序列
     */
    void createTiredSequence();
    
public:
    MultimodalFeedbackSystem();
    
    /**
     * 初始化系统
     */
    void begin(OLEDDisplay* disp, ServoController* srv);
    
    /**
     * 执行反馈场景
     */
    void executeFeedbackScenario(FeedbackScenario scenario, 
                               FeedbackAction actionType = FeedbackAction::FULL_FEEDBACK);
    
    /**
     * 执行自定义反馈序列
     */
    void executeCustomSequence(const FeedbackMotion* motions, uint8_t count);
    
    /**
     * 更新反馈状态（用于非阻塞控制）
     */
    void update();
    
    /**
     * 检查序列是否播放完成
     */
    bool isSequenceComplete() const { return !isSequencePlaying; }
    
    /**
     * 停止当前反馈序列
     */
    void stopSequence();
    
    /**
     * 生成语音文本（热反馈）
     */
    String generateThermalFeedback(float temp, float humidity);
    
    /**
     * 生成表情反馈（基于表情库）
     */
    void applyEmotionFeedback(EmotionState emotion);
};

// ==================== 预定义反馈序列 ====================

// 舞蹈反馈序列
static const FeedbackMotion DANCE_MOTIONS[] = {
    {ServoAction::SHAKE_LEFT, EmotionState::HAPPY, 300, "Dancing!", true},
    {ServoAction::SHAKE_RIGHT, EmotionState::HAPPY, 300, nullptr, false},
    {ServoAction::NOD_UP, EmotionState::HAPPY, 300, nullptr, false},
    {ServoAction::SHAKE_LEFT, EmotionState::HAPPY, 300, nullptr, false},
    {ServoAction::SHAKE_RIGHT, EmotionState::HAPPY, 300, nullptr, false},
    {ServoAction::RESET, EmotionState::HAPPY, 200, nullptr, false}
};

// 兴奋反馈序列
static const FeedbackMotion EXCITED_MOTIONS[] = {
    {ServoAction::NOD_UP, EmotionState::HAPPY, 200, "So excited!", true},
    {ServoAction::SHAKE_RIGHT, EmotionState::HAPPY, 200, nullptr, false},
    {ServoAction::NOD_UP, EmotionState::HAPPY, 200, nullptr, false},
    {ServoAction::SHAKE_LEFT, EmotionState::HAPPY, 200, nullptr, false},
    {ServoAction::RESET, EmotionState::HAPPY, 100, nullptr, false}
};

// 疲劳反馈序列
static const FeedbackMotion TIRED_MOTIONS[] = {
    {ServoAction::RESET, EmotionState::SLEEPY, 500, "So tired...", true},
    {ServoAction::NOD_DOWN, EmotionState::SLEEPY, 300, nullptr, false},
    {ServoAction::RESET, EmotionState::SLEEPY, 200, nullptr, false}
};

// 热反馈序列
static const FeedbackMotion HOT_MOTIONS[] = {
    {ServoAction::SHAKE_LEFT, EmotionState::HOT_WARNING, 300, "Too hot!", true},
    {ServoAction::SHAKE_RIGHT, EmotionState::HOT_WARNING, 300, nullptr, false},
    {ServoAction::RESET, EmotionState::HOT_WARNING, 200, nullptr, false}
};

#endif
