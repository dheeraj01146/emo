#pragma once

#include "display.h"
#include "servo_controller.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <atomic>
#include <cstring>

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

    void ResetInteraction(const char* wake_emotion = "neutral") {
        idle_seconds_ = 0;
        if (display_ != nullptr && wake_emotion != nullptr) {
            display_->SetEmotion(wake_emotion);
            ServoController::GetInstance().SetDirection(wake_emotion);
        }
    }

    void RunTaskLoop() {
        const char* last_emotion = nullptr;
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(10000)); // Har 10 second delay
            if (display_ == nullptr) continue;

            idle_seconds_ += 10;
            const char* target_emotion = "happy";

            if (idle_seconds_ < 40) {
                target_emotion = "happy";
            } else if (idle_seconds_ >= 40 && idle_seconds_ < 90) {
                target_emotion = "thinking"; // Left/Thinking look
            } else if (idle_seconds_ >= 90 && idle_seconds_ < 150) {
                target_emotion = "angry";    // Right/Angry look
            } else if (idle_seconds_ >= 150 && idle_seconds_ < 330) {
                target_emotion = "sleepy";   // Sleepy down/tilt
            } else if (idle_seconds_ >= 330 && idle_seconds_ < 370) {
                target_emotion = "neutral";  // Center
            } else {
                idle_seconds_ = 40;          // Loop reset
                target_emotion = "thinking";
            }

            // Sirf tabhi update karega jab emotion actually change ho
            if (last_emotion != target_emotion) {
                display_->SetEmotion(target_emotion);
                
                // Display ke sath servo ki direction sync kar di!
                ServoController::GetInstance().SetDirection(target_emotion);
                
                last_emotion = target_emotion;
                ESP_LOGI(MOOD_TAG, "Emotion & Servo Synced -> %s", target_emotion);
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
