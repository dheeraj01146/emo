#pragma once

#include "display.h"
#include "servo_controller.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_random.h>
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
        is_busy_ = false;

        if (task_handle_ == nullptr) {
            xTaskCreatePinnedToCore(
                [](void* arg) {
                    auto* engine = static_cast<EmoMoodEngine*>(arg);
                    engine->RunTaskLoop();
                },
                "EmoMoodTask",
                4096,
                this,
                2,
                &task_handle_,
                1
            );
            ESP_LOGI(MOOD_TAG, "Living EMO Masti Engine started!");
        }
    }

    // Jab user wake word bole ya baat shuru ho
    void ResetInteraction(const char* wake_emotion = "happy") {
        idle_seconds_ = 0;
        is_busy_ = true; // Baat karte waqt random idle bakchodi rok do
        if (display_ != nullptr && wake_emotion != nullptr) {
            display_->SetEmotion(wake_emotion);
            ServoController::GetInstance().OnListening();
        }
    }

    // Jab AI (LLM) response de aur emotion bheje -> Asli EMO ki tarah react kare!
    void OnEmotionResponse(const char* emotion) {
        idle_seconds_ = 0;
        if (display_ != nullptr && emotion != nullptr) {
            display_->SetEmotion(emotion);
            ServoController::GetInstance().ReactToEmotion(emotion);
            ESP_LOGI(MOOD_TAG, "AI Response Emotion & Move -> %s", emotion);
        }
    }

    // Jab baat khatam ho aur EMO wapas Idle/Standby mein aaye
    void SetIdleMode(bool idle) {
        is_busy_ = !idle;
        if (idle) {
            idle_seconds_ = 0;
        }
    }

    void RunTaskLoop() {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(4000)); // Har 4 second mein active check!
            if (display_ == nullptr || is_busy_) continue;

            idle_seconds_ += 4;

            // Phase 1: 0s se 150s tak -> Full Asli EMO Bakchodi / Masti Mode!
            if (idle_seconds_ >= 12 && idle_seconds_ < 150) {
                DoRandomEmoBakchodi();
            }
            // Phase 2: 150s se 330s (Pure 3 Minute) -> Deep Sleepy Mode
            else if (idle_seconds_ >= 150 && idle_seconds_ < 330) {
                if (!is_sleeping_) {
                    is_sleeping_ = true;
                    display_->SetEmotion("sleepy");
                    ServoController::GetInstance().ReactToEmotion("sleepy");
                    ESP_LOGI(MOOD_TAG, "EMO entered 3-min sleep");
                }
            }
            // Phase 3: Sleep ke theek 3 min baad (330s) -> Wake up Shake & Repeat Loop!
            else if (idle_seconds_ >= 330) {
                is_sleeping_ = false;
                idle_seconds_ = 8; // Wapas masti loop mein bhej do!
                display_->SetEmotion("neutral");
                // Uthne par gardan hila kar fresh hona
                const ServoStep wakeup_shake[] = {
                    {65.0f, 1.8f, 10},
                    {115.0f, 1.8f, 10},
                    {90.0f, 1.2f, 5}
                };
                ServoController::GetInstance().PlaySequence(wakeup_shake, 3);
                ESP_LOGI(MOOD_TAG, "EMO woke up after 3 mins! Restarting masti loop.");
            }
        }
    }

private:
    EmoMoodEngine() : display_(nullptr), task_handle_(nullptr), idle_seconds_(0), is_busy_(false), is_sleeping_(false) {}
    ~EmoMoodEngine() {
        if (task_handle_ != nullptr) {
            vTaskDelete(task_handle_);
            task_handle_ = nullptr;
        }
    }

    void DoRandomEmoBakchodi() {
        int r = esp_random() % 100;

        if (r < 22) {
            // 1. Left side curious peek (Left dekhega toh left ghumega)
            display_->SetEmotion("thinking");
            ServoController::GetInstance().SetTarget(58.0f + (esp_random() % 10), 1.4f);
        }
        else if (r < 44) {
            // 2. Right side curious peek (Right dekhega toh right ghumega)
            display_->SetEmotion("thinking");
            ServoController::GetInstance().SetTarget(112.0f + (esp_random() % 14), 1.4f);
        }
        else if (r < 60) {
            // 3. Achanak khush hoke Wiggle / Dance karna!
            display_->SetEmotion("happy");
            ServoController::GetInstance().ReactToEmotion("happy");
        }
        else if (r < 72) {
            // 4. Double-take / Surprised look (Idhar-udhar tezi se dekhna)
            display_->SetEmotion("funny");
            ServoController::GetInstance().ReactToEmotion("surprised");
        }
        else if (r < 84) {
            // 5. Bina baat thoda nakhre / gussa dikhana (Attention maangna)
            display_->SetEmotion("angry");
            ServoController::GetInstance().ReactToEmotion("angry");
        }
        else {
            // 6. Chill Center Neutral
            display_->SetEmotion("neutral");
            ServoController::GetInstance().SetTarget(90.0f, 1.0f);
        }
    }

    Display* display_;
    TaskHandle_t task_handle_;
    std::atomic<uint32_t> idle_seconds_;
    std::atomic<bool> is_busy_;
    bool is_sleeping_;
};
