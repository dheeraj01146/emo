#pragma once
#include "display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

inline void RunEmoBootAnimation(Display* display) {
    if (!display) return;

    // 1. Line-by-Line Terminal Hardware Checks
    display->SetChatMessage("system", "");
    const char* boot_lines[] = {
        "> BOOTING EMO CORE...",
        "> RAM [8MB PSRAM] ........ [ OK ]",
        "> DISPLAY SUBSYSTEM ...... [ OK ]",
        "> ENGINE SIMULATOR ....... [ OK ]",
        "> HARDWARE SERVO ......... [ OK ]",
        "> NETWORK (WIFI) ......... [ CONNECTED ]"
    };

    for (int i = 0; i < 6; i++) {
        display->SetChatMessage("system", boot_lines[i]);
        vTaskDelay(pdMS_TO_TICKS(180));
    }
    vTaskDelay(pdMS_TO_TICKS(350));

    // 2. Samsung-Style Floating Wave Effect
    for (int step = 0; step < 4; step++) {
        if (step % 2 == 0) {
            display->SetChatMessage("system", "\n   EMO IS LIVE\n   RIGHT NOW");
        } else {
            display->SetChatMessage("system", "\n\n   EMO IS LIVE\n   RIGHT NOW");
        }
        vTaskDelay(pdMS_TO_TICKS(220));
    }
    vTaskDelay(pdMS_TO_TICKS(300));

    // 3. Clear Screen & Set Normal Face
    display->SetChatMessage("system", "");
    display->SetEmotion("neutral");
}
