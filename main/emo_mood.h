#pragma once

#include "display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_log.h>
#include <atomic>

#define MOOD_TAG "EmoMood"

class EmoMoodEngine {
public:
    static EmoMoodEngine& GetInstance() {
        static EmoMoodEngine instance;
        return instance;
    }

    void Start(Display* display) {
        display_ = display;
        idle_seconds_ = 0;

        if (timer_handle_ == nullptr) {
            timer_handle_ = xTimerCreate(
                "EmoMoodTimer",
                pdMS_TO_TICKS(10000), // Har 10 second me check karega
                pdTRUE,
                this,
                [](TimerHandle_t xTimer) {
                    auto* engine = static_cast<EmoMoodEngine*>(pvTimerGetTimerID(xTimer));
                    engine->OnTimerTick();
                }
            );
            if (timer_handle_ != nullptr) {
                xTimerStart(timer_handle_, 0);
                ESP_LOGI(MOOD_TAG, "Mood engine started successfully");
            }
        }
    }

    void ResetInteraction(const char* wake_emotion = "happy") {
        idle_seconds_ = 0;
        if (display_ != nullptr && wake_emotion != nullptr) {
            display_->SetEmotion(wake_emotion);
        }
    }

    void OnTimerTick() {
        if (display_ == nullptr) return;

        idle_seconds_ += 10;

        // Stage 1: 40 seconds idle -> thoda bored/neutral expression
        if (idle_seconds_ >= 40 && idle_seconds_ < 90) {
            display_->SetEmotion("neutral");
        }
        // Stage 2: 1.5 minutes (90s) tak koi baat nahi -> sad
        else if (idle_seconds_ >= 90 && idle_seconds_ < 150) {
            display_->SetEmotion("sad");
        }
        // Stage 3: 2.5 minutes (150s) se zyada bilkul shant -> sleepy
        else if (idle_seconds_ >= 150) {
            display_->SetEmotion("sleepy");
        }
    }

private:
    EmoMoodEngine() : display_(nullptr), timer_handle_(nullptr), idle_seconds_(0) {}
    ~EmoMoodEngine() {
        if (timer_handle_ != nullptr) {
            xTimerStop(timer_handle_, 0);
            xTimerDelete(timer_handle_, 0);
        }
    }

    Display* display_;
    TimerHandle_t timer_handle_;
    std::atomic<uint32_t> idle_seconds_;
};
