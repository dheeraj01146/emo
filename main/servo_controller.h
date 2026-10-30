#pragma once

#include "driver/ledc.h"
#include <esp_random.h>

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
            .freq_hz = 50, // Standard 50Hz PWM
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

        // Head ko center position (90 degree) par set karo
        SetAngle(90.0f);
    }

    void SetAngle(float angle) {
        if (angle < 45.0f) angle = 45.0f;
        if (angle > 135.0f) angle = 135.0f;

        // 0.5ms = 0 deg, 2.5ms = 180 deg
        float pulse_us = 500.0f + (angle / 180.0f) * 2000.0f;
        uint32_t duty = (uint32_t)((pulse_us / 20000.0f) * 16384.0f);

        ledc_set_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL, duty);
        ledc_update_duty(SERVO_LEDC_MODE, SERVO_LEDC_CHANNEL);
    }

    // Natural Idle Glance (Thoda idhar udhar dekhna)
    void OnIdleTick() {
        int r = esp_random() % 100;
        if (r < 20) {
            float target = 70.0f + (esp_random() % 41); // 70 to 110 degrees
            SetAngle(target);
        }
    }

    // Listening ke time seedha center focus
    void OnListening() {
        SetAngle(90.0f);
    }

    // Speaking ke time subtle head tilt
    void OnSpeakingTick() {
        static bool toggle = false;
        toggle = !toggle;
        SetAngle(toggle ? 85.0f : 95.0f);
    }

private:
    ServoController() = default;
};
