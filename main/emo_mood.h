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
                4096,              // 4KB Safe Stack
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

            // Stage 1: 40s se 90s -> thinking
            if (idle_seconds_ >= 40 && idle_seconds_ < 90) {
                display_->SetEmotion("thinking");
            }
            // Stage 2: 90s se 150s -> angry
            else if (idle_seconds_ >= 90 && idle_seconds_ < 150) {
                display_->SetEmotion("angry");
            }
            // Stage 3: 150s se 330s (pure 3 min) -> sleepy
            else if (idle_seconds_ >= 150 && idle_seconds_ < 330) {
                display_->SetEmotion("sleepy");
            }
            // Stage 4: Sleepy ke 3 min baad (330s se 370s) -> neutral
            else if (idle_seconds_ >= 330 && idle_seconds_ < 370) {
                display_->SetEmotion("neutral");
            }
            // Loop Reset: 370s ke baad seedha wapas Stage 1 (40s) se repeat
            else if (idle_seconds_ >= 370) {
                idle_seconds_ = 40;
                display_->SetEmotion("thinking");
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
