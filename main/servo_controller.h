#pragma once

#include "driver/ledc.h"
#include "driver/gpio.h"
#include <esp_random.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <cmath>

#define SERVO_PIN           GPIO_NUM_18
// Timer 2 aur Channel 2 use karein taaki Backlight (Timer 0/1) se takraye nahi
#define SERVO_LEDC_TIMER    LEDC_TIMER_2
#define SERVO_LEDC_MODE     LEDC_LOW_SPEED_MODE
#define SERVO_LEDC_CHANNEL  LEDC_CHANNEL_2
#define SERVO_LEDC_RES      LEDC_TIMER_14_BIT

class ServoController {
public:
    static ServoController& GetInstance() {
        static ServoController instance;
        return instance;
    }

    void Initialize() {
        // Step A: GPIO 18 ko kisi bhi internal USB/default function se detach karke GPIO mode me lao
        gpio_reset_pin(SERVO_PIN);
        gpio_set_direction(SERVO_PIN, GPIO_MODE_OUTPUT);

        // Step B: Dedicated Timer 2 Setup
        ledc_timer_config_t timer_conf = {
            .speed_mode = SERVO_LEDC_MODE,
            .duty_resolution = SERVO_LEDC_RES,
            .timer_num = SERVO_LEDC_TIMER,
            .freq_hz = 50,
            .clk_cfg = LEDC_AUTO_CLK
        };
        ledc_timer_config(&timer_conf);

        // Step C: Dedicated Channel 2 Setup
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
        if (angle < 50.0f) angle = 50.0f;
        if (angle > 130.0f) angle = 130.0f;
        target_angle_ = angle;
        step_speed_ = speed;
    }

    void OnIdleTick() {
        int r = esp_random() % 100;
        if (r < 15) {
            float target = 72.0f + (esp_random() % 37);
            SetTarget(target, 0.6f);
        } else if (r < 25) {
            SetTarget(90.0f, 0.8f);
        }
    }

    void OnListening() {
        SetTarget(90.0f, 1.5f);
    }

    void OnSpeakingTick() {
        static int action_cooldown = 0;
        if (action_cooldown > 0) {
            action_cooldown--;
            return;
        }

        int r = esp_random() % 100;
        if (r < 35) {
            SetTarget(78.0f + (esp_random() % 7), 1.2f);
            action_cooldown = 1;
        } else if (r < 70) {
            SetTarget(95.0f + (esp_random() % 7), 1.2f);
            action_cooldown = 1;
        } else if (r < 85) {
            float tilt = (esp_random() % 2 == 0) ? 66.0f : 114.0f;
            SetTarget(tilt, 1.6f);
            action_cooldown = 2;
        } else {
            SetTarget(90.0f, 1.0f);
            action_cooldown = 1;
        }
    }

private:
    ServoController() = default;

    float current_angle_ = 90.0f;
    float target_angle_ = 90.0f;
    float step_speed_ = 1.0f;

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
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
};
