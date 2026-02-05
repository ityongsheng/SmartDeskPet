#include "Servo_Controller.h"

ServoController::ServoController() 
    : headPos(90), nodPos(90), actionStartTime(0), 
      currentAction(ServoAction::IDLE), isActing(false) {
}

void ServoController::begin() {
    headServo.attach(SERVO_HEAD_PIN, 500, 2500);  // SG90 脉宽范围
    nodServo.attach(SERVO_NOD_PIN, 500, 2500);
    
    // 复位到中点
    setHeadPosition(90);
    setNodPosition(90);
    delay(500);
}

void ServoController::setHeadPosition(uint16_t angle) {
    angle = constrain(angle, 0, 180);
    headPos = angle;
    headServo.write(angle);
}

void ServoController::setNodPosition(uint16_t angle) {
    angle = constrain(angle, 0, 180);
    nodPos = angle;
    nodServo.write(angle);
}

void ServoController::performAction(ServoAction action) {
    if (isActing) return;  // 已在执行动作
    
    currentAction = action;
    actionStartTime = millis();
    isActing = true;
    
    switch (action) {
        case ServoAction::SHAKE_LEFT:
            setHeadPosition(45);  // 摇到左边
            break;
        case ServoAction::SHAKE_RIGHT:
            setHeadPosition(135); // 摇到右边
            break;
        case ServoAction::NOD_UP:
            setNodPosition(30);   // 点头抬起
            break;
        case ServoAction::NOD_DOWN:
            setNodPosition(150);  // 点头低下
            break;
        case ServoAction::RESET:
            setHeadPosition(90);
            setNodPosition(90);
            break;
        default:
            isActing = false;
            break;
    }
}

void ServoController::update() {
    if (!isActing) return;
    
    // 检查动作是否完成（超时）
    if (millis() - actionStartTime >= SERVO_ACTION_DURATION) {
        // 恢复到中点
        setHeadPosition(90);
        setNodPosition(90);
        isActing = false;
        currentAction = ServoAction::IDLE;
    }
}

void ServoController::stop() {
    isActing = false;
    currentAction = ServoAction::IDLE;
    setHeadPosition(90);
    setNodPosition(90);
}
