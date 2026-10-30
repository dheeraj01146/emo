#pragma once

#include "driver/ledc.h"
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cmath>

#define SERVO_PIN           GPIO_NUM_18
#define SERVO_LEDC_TIMER    LEDC_TIMER_1
#define SERVO_LEDC_MODE     LEDC_LOW_SPEED_MODE
#define SERVO_LEDC_CHANNEL  LEDC_CHANNEL_1
#define SERVO_LEDC_RES      LEDC_TIMER_14_BIT

class ServoController {
public:
    static ServoController& GetInstance() {
        static ServoController instance;
        return instance;
    }

    void Initialize() {
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
        ApplyHardwareAngle(90.0f);

        // Smooth movement task create karo
        xTaskCreate([](void* arg) {
            static_cast<ServoController*>(arg)->SmoothTask();
        }, "servo_smooth", 2048, this, 3, nullptr);
    }

    // Direct jump nahi, target set hoga aur task smoothly le jayega
    void SetAngle(float angle) {
        if (angle < 45.0f) angle = 45.0f;
        if (angle > 135.0f) angle = 135.0f;
        target_angle_ = angle;
    }

    // Natural Idle Glance (Thoda zyada aur natural movement)
    void OnIdleTick() {
        int r = esp_random() % 100;
        if (r < 30) {
            float target = 65.0f + (esp_random() % 51); // 65 se 115 degrees
            SetAngle(target);
        }
    }

    // Listening par seedha center focus
    void OnListening() {
        SetAngle(90.0f);
    }

    // Speaking ke time broad and expressive tilt
    void OnSpeakingTick() {
        static bool toggle = false;
        toggle = !toggle;
        SetAngle(toggle ? 68.0f : 112.0f);
    }

private:
    ServoController() = default;

    float current_angle_ = 90.0f;
    float target_angle_ = 90.0f;

    void ApplyHardwareAngle(float angle) {
        float pulse_us = 500.0f + (angle / 180.0f) * 2000.0f;
        uint32_t duty = (uint32_t)((pulse_us / 20000.0f) * 16384.0f);
        ledc_set_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL, duty);
        ledc_update_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL);
    }

    void SmoothTask() {
        while (true) {
            float diff = target_angle_ - current_angle_;
            if (std::abs(diff) > 0.5f) {
                // Har 20ms me 1.5 degree ka smooth step
                float step = 1.5f;
                if (std::abs(diff) < step) {
                    current_angle_ = target_angle_;
                } else {
                    current_angle_ += (diff > 0) ? step : -step;
                }
                ApplyHardwareAngle(current_angle_);
            }
            vTaskDelay(pdMS_TO_TICKS(20)); // 50 updates per second
        }
    }
};
