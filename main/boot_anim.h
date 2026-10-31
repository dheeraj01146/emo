#pragma once

#include "display.h"
#include "audio_codec.h"
#include "assets/lang_config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#define BOOT_TAG "BootAnim"

inline void RunEmoBootAnimation(Display* display) {
    if (display == nullptr) return;

    ESP_LOGI(BOOT_TAG, "Running custom startup sequence...");

    // Stage 1: Device is booting now (3 seconds with pulsing text)
    for (int i = 0; i < 3; i++) {
        display->SetStatus("⚡ SYSTEM STARTING ⚡");
        display->SetChatMessage("assistant", "Your device is booting now .");
        vTaskDelay(pdMS_TO_TICKS(500));

        display->SetStatus("⚡ SYSTEM STARTING ⚡");
        display->SetChatMessage("assistant", "Your device is booting now ...");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // Stage 2: Screen freeze - EMO IS LIVE / Powered by DHEERAJ
    display->ClearChatMessages();
    display->SetStatus("✦ EMO AI CORE ✦");
    display->SetChatMessage("assistant", "EMO IS LIVE\nPowered by DHEERAJ");
    vTaskDelay(pdMS_TO_TICKS(1800));

    // Stage 3: Smooth Eye Unfold Animation (Iris opening effect)
    display->SetChatMessage("assistant", "[-  -]");
    vTaskDelay(pdMS_TO_TICKS(120));
    display->SetChatMessage("assistant", "[o  o]");
    vTaskDelay(pdMS_TO_TICKS(120));
    display->SetChatMessage("assistant", "[O  O]");
    vTaskDelay(pdMS_TO_TICKS(150));

    // Stage 4: Ready State Transition
    display->ClearChatMessages();
    display->SetStatus("STANDBY");
    display->SetEmotion("neutral");
}
