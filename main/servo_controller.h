#pragma once

#include "driver/ledc.h"
#include "driver/gpio.h"
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cmath>
#include <cstring>

#define SERVO_PIN           GPIO_NUM_18
#define SERVO_LEDC_TIMER    LEDC_TIMER_2
#define SERVO_LEDC_MODE     LEDC_LOW_SPEED_MODE
#define SERVO_LEDC_CHANNEL  LEDC_CHANNEL_2
#define SERVO_LEDC_RES      LEDC_TIMER_14_BIT

struct ServoStep {
    float angle;
    float speed;
    int hold_ticks; // 1 tick = 20ms (e.g., 20 ticks = 400ms)
};

class ServoController {
public:
    static ServoController& GetInstance() {
        static ServoController instance;
        return instance;
    }

    void Initialize() {
        gpio_reset_pin(SERVO_PIN);
        gpio_set_direction(SERVO_PIN, GPIO_MODE_OUTPUT);

        ledc_timer_config_t timer_conf = {
            .speed_mode = SERVO_LEDC_MODE,
            .duty_resolution = SERVO_LEDC_RES,
            .timer_num = SERVO_LEDC_TIMER,
            .freq_hz = 50,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ledc_timer_config(&timer_conf);

        ledc_channel_config_t channel_conf = {
            .gpio_num = SERVO_PIN,
            .speed_mode = SERVO_LEDC_MODE,
            .channel = SERVO_LEDC_CHANNEL,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = SERVO_LEDC_TIMER,
            .duty = 0,
            .hpoint = 0
        };
        ledc_channel_config(&channel_conf);

        current_angle_ = 90.0f;
        target_angle_ = 90.0f;
        step_speed_ = 1.2f;
        ApplyHardwareAngle(90.0f);

        xTaskCreate([](void* arg) {
            static_cast<ServoController*>(arg)->SmoothTask();
        }, "servo_smooth", 2048, this, 3, nullptr);
    }

    void SetTarget(float angle, float speed = 1.2f) {
        seq_len_ = 0; // Cancel any running sequence
        SetTargetInternal(angle, speed);
    }

    // Multi-step bakchodi / dance moves chalane ke liye
    void PlaySequence(const ServoStep* steps, int count) {
        if (count <= 0 || count > 8) return;
        for (int i = 0; i < count; i++) {
            seq_[i] = steps[i];
        }
        seq_index_ = 0;
        seq_len_ = count;
        seq_wait_ticks_ = seq_[0].hold_ticks;
        SetTargetInternal(seq_[0].angle, seq_[0].speed);
    }

    // Asli EMO ki tarah Emotion aur AI Response ke hisaab se physical reaction!
    void ReactToEmotion(const char* emotion) {
        if (emotion == nullptr) return;

        if (strcmp(emotion, "happy") == 0 || strcmp(emotion, "laughing") == 0 || strcmp(emotion, "funny") == 0) {
            // Khushi mein Left-Right Wiggle Dance!
            const ServoStep happy_wiggle[] = {
                {70.0f, 2.2f, 12},
                {110.0f, 2.2f, 12},
                {76.0f, 2.0f, 10},
                {104.0f, 2.0f, 10},
                {90.0f, 1.5f, 5}
            };
            PlaySequence(happy_wiggle, 5);
        }
        else if (strcmp(emotion, "angry") == 0 || strcmp(emotion, "frustrated") == 0) {
            // Gusse mein sharp aggressive head shake!
            const ServoStep angry_shake[] = {
                {122.0f, 2.8f, 10},
                {58.0f, 2.8f, 10},
                {115.0f, 2.5f, 10},
                {90.0f, 1.8f, 5}
            };
            PlaySequence(angry_shake, 4);
        }
        else if (strcmp(emotion, "thinking") == 0 || strcmp(emotion, "confused") == 0) {
            // Sochte waqt pehle Left dekhega, rukega, phir halka Right
            const ServoStep think_look[] = {
                {62.0f, 1.0f, 45},  // Slow left look & hold
                {108.0f, 0.9f, 35}, // Slow right glance
                {75.0f, 1.0f, 20}
            };
            PlaySequence(think_look, 3);
        }
        else if (strcmp(emotion, "surprised") == 0 || strcmp(emotion, "shocked") == 0) {
            // Double-take! (Jhatke se idhar-udhar dekhna)
            const ServoStep double_take[] = {
                {55.0f, 3.2f, 8},
                {125.0f, 3.2f, 12},
                {90.0f, 2.5f, 5}
            };
            PlaySequence(double_take, 3);
        }
        else if (strcmp(emotion, "cool") == 0 || strcmp(emotion, "wink") == 0 || strcmp(emotion, "confident") == 0) {
            // Swag side-pose
            const ServoStep cool_pose[] = {
                {118.0f, 1.6f, 40},
                {82.0f, 1.2f, 20},
                {90.0f, 1.0f, 5}
            };
            PlaySequence(cool_pose, 3);
        }
        else if (strcmp(emotion, "sad") == 0 || strcmp(emotion, "crying") == 0) {
            // Dheere se gardan ek taraf jhuka lena
            SetTarget(68.0f, 0.5f);
        }
        else if (strcmp(emotion, "sleepy") == 0) {
            // Sote waqt slow resting corner
            SetTarget(78.0f, 0.4f);
        }
        else if (strcmp(emotion, "left") == 0) {
            SetTarget(58.0f, 1.5f);
        }
        else if (strcmp(emotion, "right") == 0) {
            SetTarget(122.0f, 1.5f);
        }
        else {
            // Neutral / Center
            SetTarget(90.0f, 1.2f);
        }
    }

    // Backwards compatibility ke liye
    void SetDirection(const char* emotion) {
        ReactToEmotion(emotion);
    }

    void OnIdleTick() {
        // Idle movement ab EmoMoodEngine khud display ke sath sync karke chalayega
        // Taaki display aur servo alag-alag na bhaagein!
    }

    void OnListening() {
        // Jab user bole, toh turant seedha hoke dhyan se sune
        const ServoStep alert_listen[] = {
            {96.0f, 2.2f, 6},
            {90.0f, 1.8f, 5}
        };
        PlaySequence(alert_listen, 2);
    }

    void OnSpeakingTick() {
        // Bolte waqt natural gesturing
        if (seq_len_ > 0) return; // Agar emotion sequence chal raha hai toh usko mat kaato

        static int action_cooldown = 0;
        if (action_cooldown > 0) {
            action_cooldown--;
            return;
        }

        int r = esp_random() % 100;
        if (r < 35) {
            SetTarget(74.0f + (esp_random() % 10), 1.4f);
            action_cooldown = 1;
        } else if (r < 70) {
            SetTarget(96.0f + (esp_random() % 10), 1.4f);
            action_cooldown = 1;
        } else if (r < 88) {
            float tilt = (esp_random() % 2 == 0) ? 64.0f : 116.0f;
            SetTarget(tilt, 1.8f);
            action_cooldown = 2;
        } else {
            SetTarget(90.0f, 1.2f);
            action_cooldown = 1;
        }
    }

private:
    ServoController() = default;

    float current_angle_ = 90.0f;
    float target_angle_ = 90.0f;
    float step_speed_ = 1.2f;

    ServoStep seq_[8];
    int seq_len_ = 0;
    int seq_index_ = 0;
    int seq_wait_ticks_ = 0;

    void SetTargetInternal(float angle, float speed) {
        if (angle < 50.0f) angle = 50.0f;
        if (angle > 130.0f) angle = 130.0f;
        target_angle_ = angle;
        step_speed_ = speed;
    }

    void ApplyHardwareAngle(float angle) {
        float pulse_us = 500.0f + (angle / 180.0f) * 2000.0f;
        uint32_t duty = (uint32_t)((pulse_us / 20000.0f) * 16384.0f);
        ledc_set_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL, duty);
        ledc_update_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL);
    }

    void SmoothTask() {
        while (true) {
            float diff = target_angle_ - current_angle_;
            if (std::abs(diff) > 0.3f) {
                if (std::abs(diff) < step_speed_) {
                    current_angle_ = target_angle_;
                } else {
                    current_angle_ += (diff > 0) ? step_speed_ : -step_speed_;
                }
                ApplyHardwareAngle(current_angle_);
            } else if (seq_len_ > 0) {
                // Target angle par pahunch gaya, ab hold_ticks wait karke agla step chalao
                if (seq_wait_ticks_ > 0) {
                    seq_wait_ticks_--;
                } else {
                    seq_index_++;
                    if (seq_index_ < seq_len_) {
                        seq_wait_ticks_ = seq_[seq_index_].hold_ticks;
                        SetTargetInternal(seq_[seq_index_].angle, seq_[seq_index_].speed);
                    } else {
                        seq_len_ = 0; // Sequence khatam
                    }
                }
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
};
