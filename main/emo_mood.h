#pragma once

#include "display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
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

        if (task_handle_ == nullptr) {
            xTaskCreatePinnedToCore(
                [](void* arg) {
                    auto* engine = static_cast<EmoMoodEngine*>(arg);
                    engine->RunTaskLoop();
                },
                "EmoMoodTask",
                4096,              // 4KB Safe Stack (Stack Overflow fix)
                this,
                2,                 // Task Priority
                &task_handle_,
                1                  // Core 1 par pin
            );
            ESP_LOGI(MOOD_TAG, "Mood engine task started successfully with 4KB stack");
        }
    }

    void ResetInteraction(const char* wake_emotion = "happy") {
        idle_seconds_ = 0;
        if (display_ != nullptr && wake_emotion != nullptr) {
            display_->SetEmotion(wake_emotion);
        }
    }

    void RunTaskLoop() {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(10000)); // Har 10 second delay
            if (display_ == nullptr) continue;

            idle_seconds_ += 10;

            // Stage 1: 40 seconds idle -> funny / neutral
            if (idle_seconds_ >= 40 && idle_seconds_ < 90) {
                display_->SetEmotion("funny"); // Lowercase "funny"
            }
            // Stage 2: 90 seconds (1.5 min) idle -> sad
            else if (idle_seconds_ >= 90 && idle_seconds_ < 150) {
                display_->SetEmotion("sad");
            }
            // Stage 3: 150 seconds (2.5 min) idle -> sleepy
            else if (idle_seconds_ >= 150) {
                display_->SetEmotion("sleepy");
            }
        }
    }

private:
    EmoMoodEngine() : display_(nullptr), task_handle_(nullptr), idle_seconds_(0) {}
    ~EmoMoodEngine() {
        if (task_handle_ != nullptr) {
            vTaskDelete(task_handle_);
            task_handle_ = nullptr;
        }
    }

    Display* display_;
    TaskHandle_t task_handle_;
    std::atomic<uint32_t> idle_seconds_;
};
