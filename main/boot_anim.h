#pragma once
#include "display.h"
#include <esp_lvgl_port.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

inline void RunEmoBootAnimation(Display* display) {
    if (!display) return;

    // 1. Lock LVGL display
    lvgl_port_lock(0);

    // Fullscreen Pure Black Background container
    lv_obj_t* boot_screen = lv_obj_create(lv_layer_top());
    lv_obj_set_size(boot_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(boot_screen, 0, 0);
    lv_obj_set_style_radius(boot_screen, 0, 0);
    lv_obj_set_style_pad_all(boot_screen, 15, 0);
    lv_obj_clear_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Label for Terminal Logs
    lv_obj_t* label = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0); // Pure White Retro Text
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 0, 10);
    lv_label_set_text(label, "");

    lvgl_port_unlock();

    // 2. Line-by-Line Terminal Checks (Retro Cyberpunk)
    const char* boot_lines[] = {
        "> BOOTING EMO CORE...\n",
        "> RAM [8MB PSRAM] ........ [ OK ]\n",
        "> DISPLAY SUBSYSTEM ...... [ OK ]\n",
        "> ENGINE SIMULATOR ....... [ OK ]\n",
        "> HARDWARE SERVO ......... [ OK ]\n",
        "> NETWORK (WIFI) ......... [ CONNECTED ]\n"
    };

    char log_buffer[512] = "";
    for (int i = 0; i < 6; i++) {
        strcat(log_buffer, boot_lines[i]);
        lvgl_port_lock(0);
        lv_label_set_text(label, log_buffer);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(180));
    }
    vTaskDelay(pdMS_TO_TICKS(400));

    // 3. Clear Screen for Samsung-Style Centered Wave
    lvgl_port_lock(0);
    lv_obj_clean(boot_screen);

    lv_obj_t* live_label = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(live_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(live_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(live_label, "EMO IS LIVE\nRIGHT NOW");
    lv_obj_align(live_label, LV_ALIGN_CENTER, 0, 0);
    lvgl_port_unlock();

    // Breathing / Shimmer Wave Effect
    for (int step = 0; step < 4; step++) {
        lvgl_port_lock(0);
        if (step % 2 == 0) {
            lv_obj_align(live_label, LV_ALIGN_CENTER, 0, -4);
            lv_obj_set_style_text_opa(live_label, LV_OPA_COVER, 0);
        } else {
            lv_obj_align(live_label, LV_ALIGN_CENTER, 0, 4);
            lv_obj_set_style_text_opa(live_label, LV_OPA_70, 0);
        }
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(220));
    }
    vTaskDelay(pdMS_TO_TICKS(300));

    // 4. Destroy Boot Screen & Transition to EMO Face
    lvgl_port_lock(0);
    lv_obj_del(boot_screen);
    lvgl_port_unlock();

    display->SetEmotion("neutral");
}
