#pragma once
#include "display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_log.h>

class EmoMoodEngine {
public:
    static EmoMoodEngine& GetInstance() {
        static EmoMoodEngine instance;
        return instance;
    }

    void Start(Display* display) {
        display_ = display;
        idle_seconds_ = 0;
        hunger_level_ = 0;

        // Har 10 second par background timer chalega
        mood_timer_ = xTimerCreate("EmoMoodTimer", pdMS_TO_TICKS(10000), pdTRUE, this, [](TimerHandle_t xTimer) {
            auto engine = static_cast<EmoMoodEngine*>(pvTimerGetTimerID(xTimer));
            engine->OnTick();
        });

        if (mood_timer_ != nullptr) {
            xTimerStart(mood_timer_, 0);
        }
    }

    // Jab bhi robot se baat ho ya tap ho, timer reset hoga
    void ResetInteraction(const char* emotion = "happy") {
        idle_seconds_ = 0;
        hunger_level_ = 0;
        if (display_) {
            display_->SetEmotion(emotion);
        }
    }

    // Feeding voice command ya pet ke liye
    void Feed() {
        idle_seconds_ = 0;
        hunger_level_ = 0;
        if (display_) {
            display_->SetEmotion("love");
        }
    }

private:
    EmoMoodEngine() : display_(nullptr), mood_timer_(nullptr), idle_seconds_(0), hunger_level_(0) {}

    void OnTick() {
        if (!display_) return;

        idle_seconds_ += 10;
        hunger_level_ += 1;

        // 1. Agar 40 second tak koi na bole -> Bored / Doubt expression
        if (idle_seconds_ == 40) {
            display_->SetEmotion("engry");
        }
        // 2. Agar 1.5 min tak koi dhyan na de -> Bhookh / Sad face
        else if (hunger_level_ >= 10 && hunger_level_ < 16) {
            display_->SetEmotion("sad");
        }
        // 3. Agar 2 minute se zyada chup chap pada rahe -> Sleepy mode
        else if (idle_seconds_ >= 120) {
            display_->SetEmotion("sleepy");
        }
    }

    Display* display_;
    TimerHandle_t mood_timer_;
    int idle_seconds_;
    int hunger_level_;
};
