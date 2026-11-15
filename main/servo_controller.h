#pragma once

#include "driver/ledc.h"
#include "driver/gpio.h"
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cmath>
#include <cstring>

#define SERVO_PIN            GPIO_NUM_18
#define SERVO_LEDC_TIMER     LEDC_TIMER_2
#define SERVO_LEDC_MODE      LEDC_LOW_SPEED_MODE
#define SERVO_LEDC_CHANNEL   LEDC_CHANNEL_2
#define SERVO_LEDC_RES       LEDC_TIMER_14_BIT

struct ServoStep {
    float angle;
    float speed;
    int hold_ticks; // 1 tick = 20ms
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
        step_speed_ = 1.0f;
        ApplyHardwareAngle(90.0f);

        xTaskCreate([](void* arg) {
            static_cast<ServoController*>(arg)->SmoothTask();
        }, "servo_smooth", 2048, this, 3, nullptr);
    }

    void SetTarget(float angle, float speed = 1.0f) {
        seq_len_ = 0; 
        SetTargetInternal(angle, speed);
    }

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

    void ReactToEmotion(const char* emotion) {
        if (emotion == nullptr) return;

        // Wajan aur load bachane ke liye saare angles ko 82° aur 98° ke beech rakha hai
        if (strcmp(emotion, "happy") == 0 || strcmp(emotion, "laughing") == 0 || strcmp(emotion, "funny") == 0) {
            const ServoStep happy_wiggle[] = {
                {85.0f, 1.5f, 10},
                {95.0f, 1.5f, 10},
                {90.0f, 1.2f, 5}
            };
            PlaySequence(happy_wiggle, 3);
        }
        else if (strcmp(emotion, "angry") == 0 || strcmp(emotion, "frustrated") == 0) {
            const ServoStep angry_shake[] = {
                {86.0f, 2.0f, 8},
                {94.0f, 2.0f, 8},
                {90.0f, 1.5f, 5}
            };
            PlaySequence(angry_shake, 3);
        }
        else if (strcmp(emotion, "thinking") == 0 || strcmp(emotion, "confused") == 0) {
            const ServoStep think_look[] = {
                {85.0f, 0.8f, 30},
                {95.0f, 0.8f, 30},
                {90.0f, 1.0f, 5}
            };
            PlaySequence(think_look, 3);
        }
        else if (strcmp(emotion, "surprised") == 0 || strcmp(emotion, "shocked") == 0) {
            const ServoStep double_take[] = {
                {84.0f, 2.2f, 6},
                {96.0f, 2.2f, 8},
                {90.0f, 1.5f, 5}
            };
            PlaySequence(double_take, 3);
        }
        else if (strcmp(emotion, "cool") == 0 || strcmp(emotion, "wink") == 0 || strcmp(emotion, "confident") == 0) {
            const ServoStep cool_pose[] = {
                {94.0f, 1.2f, 30},
                {90.0f, 1.0f, 5}
            };
            PlaySequence(cool_pose, 2);
        }
        else if (strcmp(emotion, "sad") == 0 || strcmp(emotion, "crying") == 0) {
            SetTarget(86.0f, 0.5f);
        }
        else if (strcmp(emotion, "sleepy") == 0) {
            SetTarget(88.0f, 0.4f);
        }
        else if (strcmp(emotion, "left") == 0) {
            SetTarget(85.0f, 1.2f);
        }
        else if (strcmp(emotion, "right") == 0) {
            SetTarget(95.0f, 1.2f);
        }
        else {
            SetTarget(90.0f, 1.0f);
        }
    }

    void SetDirection(const char* emotion) {
        ReactToEmotion(emotion);
    }

    void OnIdleTick() {}

    void OnListening() {
        SetTarget(90.0f, 1.5f);
    }

    void OnSpeakingTick() {
        if (seq_len_ > 0) return; 

        static int action_cooldown = 0;
        if (action_cooldown > 0) {
            action_cooldown--;
            return;
        }

        // Bolte waqt movement ki frequency aur range bohot kam kar di hai
        int r = esp_random() % 100;
        if (r < 20) {
            SetTarget(88.0f + (esp_random() % 3), 1.0f);
            action_cooldown = 3; // Thoda lamba gap rakha hai taaki motor continuous na gume
        } else if (r < 40) {
            SetTarget(90.0f + (esp_random() % 3), 1.0f);
            action_cooldown = 3;
        } else {
            SetTarget(90.0f, 1.0f);
            action_cooldown = 2;
        }
    }

private:
    ServoController() = default;

    float current_angle_ = 90.0f;
    float target_angle_ = 90.0f;
    float step_speed_ = 1.0f;

    ServoStep seq_[8];
    int seq_len_ = 0;
    int seq_index_ = 0;
    int seq_wait_ticks_ = 0;

    void SetTargetInternal(float angle, float speed) {
        // HARD SAFETY LIMIT: Motor ko 80° se 100° ke bahar jaane hi nahi dega, wajan safe rahega!
        if (angle < 80.0f) angle = 80.0f;
        if (angle > 100.0f) angle = 100.0f;
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
                if (seq_wait_ticks_ > 0) {
                    seq_wait_ticks_--;
                } else {
                    seq_index_++;
                    if (seq_index_ < seq_len_) {
                        seq_wait_ticks_ = seq_[seq_index_].hold_ticks;
                        SetTargetInternal(seq_[seq_index_].angle, seq_[seq_index_].speed);
                    } else {
                        seq_len_ = 0; 
                    }
                }
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
};
