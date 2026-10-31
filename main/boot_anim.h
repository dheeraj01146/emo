#pragma once

#include "display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#define BOOT_TAG "CinematicBoot"

inline void RunEmoBootAnimation(Display* display) {
    if (display == nullptr) return;

    ESP_LOGI(BOOT_TAG, "Running cinematic glow startup...");

    // Stage 1: Pitch Black Canvas
    display->ClearChatMessages();
    display->SetStatus("");
    display->SetEmotion("neutral");

    // Stage 2: Light Flare & Expanding Circle Effect (HarmonyOS Style)
    display->SetChatMessage("assistant", "        .\n\n");
    vTaskDelay(pdMS_TO_TICKS(180));
    display->SetChatMessage("assistant", "       ( o )\n\n");
    vTaskDelay(pdMS_TO_TICKS(180));
    display->SetChatMessage("assistant", "     ((  O  ))\n\n");
    vTaskDelay(pdMS_TO_TICKS(220));

    // Stage 3: Logo Reveal with Glow Flare
    display->SetChatMessage("assistant", "━ ━ ━ ━ ━ ━ ━ ━\n   E  M  O\n━ ━ ━ ━ ━ ━ ━ ━");
    vTaskDelay(pdMS_TO_TICKS(600));

    // Stage 4: Steady Reveal - Powered by DHEERAJ
    display->SetChatMessage("assistant", "   ◆  E M O  ◆\n\n  A I   R O B O T\n\npowered by DHEERAJ");
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Stage 5: Cinematic Iris Wipe (Eyes Unfold)
    display->SetChatMessage("assistant", "—           —");
    vTaskDelay(pdMS_TO_TICKS(120));
    display->SetChatMessage("assistant", "▬           ▬");
    vTaskDelay(pdMS_TO_TICKS(120));
    display->SetChatMessage("assistant", "●           ●");
    vTaskDelay(pdMS_TO_TICKS(180));

    // Stage 6: Final Ready State
    display->ClearChatMessages();
    display->SetStatus("EMO ONLINE");
    display->SetEmotion("happy");
}
