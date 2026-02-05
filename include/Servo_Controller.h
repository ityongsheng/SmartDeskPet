#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "config.h"

/**
 * 舵机动作类型
 */
enum class ServoAction {
    SHAKE_LEFT,    // 摇头左转
    SHAKE_RIGHT,   // 摇头右转
    NOD_UP,        // 点头抬头
    NOD_DOWN,      // 点头低头
    RESET,         // 复位到中点
    IDLE           // 空闲
};

/**
 * 舵机控制类
 * 管理两个SG90舵机：摇头（180°）和点头（180°）
 */
class ServoController {
private:
    Servo headServo;    // 摇头舵机
    Servo nodServo;     // 点头舵机
    
    uint16_t headPos;   // 摇头当前位置
    uint16_t nodPos;    // 点头当前位置
    
    unsigned long actionStartTime;
    ServoAction currentAction;
    bool isActing;
    
public:
    ServoController();
    
    /**
     * 初始化舵机
     */
    void begin();
    
    /**
     * 执行舵机动作
     * @param action 要执行的动作类型
     */
    void performAction(ServoAction action);
    
    /**
     * 更新舵机状态（用于非阻塞控制）
     * 应该在主循环中定期调用
     */
    void update();
    
    /**
     * 检查是否正在执行动作
     */
    bool isPerforming() const { return isActing; }
    
    /**
     * 立即停止所有舵机动作并复位
     */
    void stop();
    
    /**
     * 设置摇头舵机位置（0-180）
     */
    void setHeadPosition(uint16_t angle);
    
    /**
     * 设置点头舵机位置（0-180）
     */
    void setNodPosition(uint16_t angle);
    
    /**
     * 获取摇头舵机当前位置
     */
    uint16_t getHeadPosition() const { return headPos; }
    
    /**
     * 获取点头舵机当前位置
     */
    uint16_t getNodPosition() const { return nodPos; }
};

#endif
