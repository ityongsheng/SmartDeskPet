# SmartDesk Phase 4 快速参考 - Quick Reference

## 🎯 核心API快速查询

### 表情库 (EmotionBitmapLibrary)

```cpp
// 初始化
emotionLibrary.begin(&display);

// 显示表情 (32x32)
emotionLibrary.drawEmotion(EmotionState::HAPPY, x, y);
emotionLibrary.drawEmotion(EmotionState::SAD, x, y);
emotionLibrary.drawEmotion(EmotionState::ANGRY, x, y);
emotionLibrary.drawEmotion(EmotionState::CONFUSED, x, y);
emotionLibrary.drawEmotion(EmotionState::HOT_WARNING, x, y);
emotionLibrary.drawEmotion(EmotionState::COLD_WARNING, x, y);
emotionLibrary.drawEmotion(EmotionState::SLEEPY, x, y);
emotionLibrary.drawEmotion(EmotionState::ALERT, x, y);

// 显示符号 (16x16)
emotionLibrary.drawSymbol("heart", x, y);
emotionLibrary.drawSymbol("sun", x, y);
emotionLibrary.drawSymbol("cloud", x, y);
emotionLibrary.drawSymbol("rain", x, y);

// 带效果显示
emotionLibrary.drawEmotionWithEffect(EmotionState::HAPPY, x, y, true);  // 反色
```

### 多模态反馈系统 (MultimodalFeedbackSystem)

```cpp
// 初始化
multimodalFeedback.begin(&display, &servo);

// 执行反馈场景
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::DANCE);
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::EXCITED);
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::TIRED);
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::HOT);
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::COLD);
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::CONFUSED);
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::CELEBRATE);
multimodalFeedback.executeFeedbackScenario(FeedbackScenario::ALERT);

// 执行自定义序列
multimodalFeedback.executeCustomSequence(motionArray, arraySize);

// 生成温度反馈文本
String feedback = multimodalFeedback.generateThermalFeedback(temp, humidity);

// 应用表情反馈
multimodalFeedback.applyEmotionFeedback(EmotionState::HAPPY);

// 更新反馈状态
multimodalFeedback.update();

// 检查序列完成
if (multimodalFeedback.isSequenceComplete()) {
    // 反馈完成
}

// 停止当前序列
multimodalFeedback.stopSequence();
```

### 舵机控制 (ServoController)

```cpp
// 初始化
servoController.begin(pin);

// 执行动作
servoController.performAction(ServoAction::SHAKE_LEFT);
servoController.performAction(ServoAction::SHAKE_RIGHT);
servoController.performAction(ServoAction::NOD_UP);
servoController.performAction(ServoAction::NOD_DOWN);
servoController.performAction(ServoAction::TILT_LEFT);
servoController.performAction(ServoAction::TILT_RIGHT);
servoController.performAction(ServoAction::RESET);
```

---

## 📋 常用代码片段

### 响应语音命令"跳个舞"

```cpp
if (voiceCommand == VOICE_DANCE) {
    multimodalFeedback.executeFeedbackScenario(
        FeedbackScenario::DANCE,
        FeedbackAction::FULL_FEEDBACK
    );
    asrproModule.speak("Let's dance!");
}
```

### 响应温度>32°C

```cpp
if (temperature > 32.0) {
    multimodalFeedback.executeFeedbackScenario(
        FeedbackScenario::HOT,
        FeedbackAction::FULL_FEEDBACK
    );
    String msg = multimodalFeedback.generateThermalFeedback(temperature, humidity);
    asrproModule.speak(msg.c_str());
}
```

### MQTT远程控制舞蹈

```cpp
// 接收 {"cmd":"emotion","emotion":"dance"}
if (strcmp(cmd, "emotion") == 0 && strcmp(emotion, "dance") == 0) {
    multimodalFeedback.executeFeedbackScenario(FeedbackScenario::DANCE);
}
```

### 显示自定义表情序列

```cpp
const FeedbackMotion sequence[] = {
    {ServoAction::SHAKE_LEFT, EmotionState::HAPPY, 300, "Left!", true},
    {ServoAction::SHAKE_RIGHT, EmotionState::HAPPY, 300, "Right!", true},
    {ServoAction::RESET, EmotionState::HAPPY, 200, nullptr, false}
};

multimodalFeedback.executeCustomSequence(
    sequence, 
    sizeof(sequence) / sizeof(sequence[0])
);
```

---

## 🎨 表情对应表

| 代码 | 表情 | 用途 | 触发条件 |
|------|------|------|---------|
| `HAPPY` | 😊 | 快乐、兴奋 | 语音"好"、天气晴朗 |
| `SAD` | 😢 | 伤心、失望 | 语音"伤心"、故障检测 |
| `ANGRY` | 😠 | 生气、不满 | 语音"生气"、错误命令 |
| `CONFUSED` | 😕 | 困惑、思考 | 不清楚命令、等待输入 |
| `HOT_WARNING` | 🔥 | 高温预警 | 温度>32°C |
| `COLD_WARNING` | ❄️ | 低温预警 | 温度<10°C |
| `SLEEPY` | 😴 | 疲劳困倦 | 长时间无活动、语音"困" |
| `ALERT` | ⚠️ | 警告异常 | 异常检测、系统警告 |

---

## 🔄 工作流示例

### 完整的多模态反馈流程

```
用户语音: "跳个舞"
    ↓
ASRPRO识别 (VOICE_DANCE)
    ↓
handleVoiceCommand()
    ↓
multimodalFeedback.executeFeedbackScenario(DANCE)
    ↓
舵机动作序列:
  - SHAKE_LEFT + HAPPY表情 (300ms)
  - SHAKE_RIGHT + HAPPY表情 (300ms)
  - NOD_UP + HAPPY表情 (300ms)
  - SHAKE_LEFT + HAPPY表情 (300ms)
  - SHAKE_RIGHT + HAPPY表情 (300ms)
  - RESET (200ms)
    ↓
asrproModule.speak("Dancing!")
    ↓
反馈完成，返回待机状态
```

### 温度阈值联动流程

```
DHT读取温度: 35°C
    ↓
checkTemperatureThreshold(35.0)
    ↓
温度 > 32°C → 触发HOT场景
    ↓
multimodalFeedback.executeFeedbackScenario(HOT)
    ↓
显示流汗表情 + 舵机摇晃
    ↓
生成反馈文本: "Too hot! Stay cool!"
    ↓
asrproModule.speak("Too hot! Stay cool!")
    ↓
MQTT发布温度状态
```

### MQTT远程控制流程

```
小程序发送: {"cmd":"emotion","emotion":"dance"}
    ↓
MQTT回调接收
    ↓
JSON解析: cmd="emotion", emotion="dance"
    ↓
multimodalFeedback.executeFeedbackScenario(DANCE)
    ↓
执行舞蹈反馈序列
    ↓
生成状态JSON响应:
{
  "emotion": "dance",
  "temperature": 25.5,
  "humidity": 45.2,
  "status": "executing"
}
    ↓
发布到 /device/status 主题
```

---

## 📊 内存使用预估

| 组件 | 大小 | 存储位置 |
|------|------|---------|
| 表情库 | 3KB | PROGMEM |
| 符号库 | 1KB | PROGMEM |
| MultimodalFeedback对象 | 200B | RAM |
| EmotionBitmapLibrary对象 | 10B | RAM |
| JSON文档缓冲 | 256B | RAM |
| **总计** | **~4.5KB** | - |

---

## ⚡ 执行时间预估

| 操作 | 时间 |
|------|------|
| 显示表情 | ~50ms |
| 执行舵机动作 | ~300-500ms |
| 语音播报 | ~1000ms |
| JSON解析 | ~50ms |
| MQTT发布 | ~100ms |
| **完整舞蹈反馈** | **~3500ms** |

---

## 🔧 调试命令

### 串口调试输出示例

```
[Setup] SmartDesk Multimodal Feedback System Starting...
[Setup] Initializing Phase 1 (Local Drivers)...
  ✓ DHT Manager initialized
  ✓ OLED Display initialized
  ✓ Servo Controller initialized
  ✓ ASRPRO Module initialized
  ✓ Emotion Bitmap Library initialized
[Setup] Initializing Phase 2 (Cloud Connection)...
  ✓ WiFi connected
  ✓ MQTT Manager initialized
[Setup] Initializing Phase 3 (Online Services)...
  ✓ Weather Service initialized
[Setup] Initializing Phase 4 (Multimodal Feedback)...
  ✓ Multimodal Feedback System initialized
  ✓ State Machine initialized

[Voice] Command recognized: 0
→ Execute DANCE feedback
[Feedback] Executing scenario: 0
[Feedback] Showing HAPPY emotion
[Feedback] Starting custom sequence with 6 motions
```

---

## 💡 最佳实践

### ✅ 推荐做法

```cpp
// 1. 在loop中调用update()保持非阻塞
void loop() {
    multimodalFeedback.update();  // 每次都调用
}

// 2. 使用FeedbackAction控制反馈类型
multimodalFeedback.executeFeedbackScenario(
    FeedbackScenario::HOT,
    FeedbackAction::FULL_FEEDBACK  // 明确指定
);

// 3. 检查序列完成状态
if (multimodalFeedback.isSequenceComplete()) {
    // 继续下一个操作
}

// 4. 合理设置阈值延迟
static unsigned long lastAlertTime = 0;
if (millis() - lastAlertTime > 30000) {  // 30秒最多一次
    // 触发警告
}
```

### ❌ 避免做法

```cpp
// 1. 不要在中断中调用反馈系统
void ISR_Handler() {
    // ❌ 错误: 在中断中执行反馈
    multimodalFeedback.executeFeedbackScenario(FeedbackScenario::DANCE);
}

// 2. 不要使用阻塞的delay()
// ❌ 错误: 整个系统都会阻塞
delay(1000);

// 3. 不要频繁创建/销毁对象
// ❌ 错误: 内存泄漏和碎片化
MultimodalFeedbackSystem* feedback = new MultimodalFeedbackSystem();
// ... 使用后没有delete

// 4. 不要忽视PROGMEM存储
// ❌ 错误: 浪费RAM
uint8_t bitmap[] = {0x00, 0x01, ...};  // 应该是PROGMEM
```

---

## 🎓 学习路径

### 初级：基础表情显示

```cpp
emotionLibrary.drawEmotion(EmotionState::HAPPY, 48, 24);
```

### 中级：语音触发反馈

```cpp
if (voiceCommand == VOICE_DANCE) {
    multimodalFeedback.executeFeedbackScenario(FeedbackScenario::DANCE);
}
```

### 高级：自定义反馈序列

```cpp
const FeedbackMotion custom[] = {
    {ServoAction::SHAKE_LEFT, EmotionState::HAPPY, 300, "Left!", true},
    {ServoAction::SHAKE_RIGHT, EmotionState::HAPPY, 300, "Right!", true},
    {ServoAction::RESET, EmotionState::HAPPY, 200, nullptr, false}
};
multimodalFeedback.executeCustomSequence(custom, 3);
```

### 专家：MQTT集成与分布式控制

```cpp
// 完整的远程控制系统
// 参考 Phase4_Integration_Example.cpp
```

---

## 📞 常见问题

**Q: 表情不显示？**
A: 检查PROGMEM声明和U8G2初始化是否正确

**Q: 舵机抖动？**
A: 检查电源供应和PWM信号频率

**Q: MQTT连接断开？**
A: 检查WiFi信号强度，增加重连延迟

**Q: 语音反馈延迟大？**
A: 使用非阻塞设计，缩短loop()周期

**Q: 内存不足？**
A: 验证所有大型数据都使用PROGMEM存储

---

## 📎 文件清单

| 文件 | 用途 | 代码行数 |
|------|------|---------|
| `Emotion_Bitmap_Library.h` | 表情库定义 | ~350 |
| `Multimodal_Feedback.h` | 多模态反馈系统 | ~150 |
| `Multimodal_Feedback.cpp` | 多模态反馈实现 | ~200 |
| `Phase4_Integration_Example.cpp` | 完整集成示例 | ~400 |
| `PHASE4_INTEGRATION_GUIDE.md` | 详细文档 | - |
| `PHASE4_QUICK_REFERENCE.md` | 本文档 | - |

---

**Last Updated: 2024**
**Status: ✅ Complete & Ready for Use**
