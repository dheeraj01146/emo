#pragma once
#include "display.h"
#include <esp_lvgl_port.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

inline void RunEmoBootAnimation(Display* display) {
    if (!display) return;

    // 1. Lock LVGL display & create Fullscreen Pure Black Screen
    lvgl_port_lock(0);

    lv_obj_t* boot_screen = lv_obj_create(lv_layer_top());
    lv_obj_set_size(boot_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(boot_screen, 0, 0);
    lv_obj_set_style_radius(boot_screen, 0, 0);
    lv_obj_set_style_pad_all(boot_screen, 16, 0);
    lv_obj_clear_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Terminal Diagnostics Text Object
    lv_obj_t* log_label = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(log_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(log_label, LV_ALIGN_TOP_LEFT, 0, 10);
    lv_label_set_text(log_label, "");

    lvgl_port_unlock();

    // 2. Slow & Heavy Terminal Hardware Checks
    const char* boot_lines[] = {
        "> INITIALIZING EMO CORE...\n",
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
        lv_label_set_text(log_label, log_buffer);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(650)); // Slow readable terminal line pacing
    }
    
    // Hold completed checks on screen for 1.2 seconds
    vTaskDelay(pdMS_TO_TICKS(1200));

    // 3. Clear Screen & Prepare Smartphone Center Logo + Bottom Branding
    lvgl_port_lock(0);
    lv_obj_clean(boot_screen);

    // Center Big Logo: EMO AI ROBOT
    lv_obj_t* center_logo = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(center_logo, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(center_logo, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(center_logo, "E M O\nAI ROBOT");
    lv_obj_align(center_logo, LV_ALIGN_CENTER, 0, -25);
    lv_obj_set_style_text_opa(center_logo, 0, 0); // Start hidden for smooth fade-in

    // Bottom Branding: powered by DHEERAJ
    lv_obj_t* bottom_brand = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(bottom_brand, lv_color_hex(0x999999), 0); // Subtle metallic grey
    lv_obj_set_style_text_align(bottom_brand, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(bottom_brand, "powered by\nD H E E R A J");
    lv_obj_align(bottom_brand, LV_ALIGN_BOTTOM_MID, 0, -18);
    lv_obj_set_style_text_opa(bottom_brand, 0, 0); // Start hidden

    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(500)); // Suspense pause in pitch dark

    // 4. Ultra Smooth Honor 7C Breathing Wave (Slow Pulses)
    for (int cycle = 0; cycle < 2; cycle++) {
        // Slow Smooth Fade-In
        for (int opa = 20; opa <= 255; opa += 15) {
            lvgl_port_lock(0);
            lv_obj_set_style_text_opa(center_logo, opa, 0);
            lv_obj_set_style_text_opa(bottom_brand, (opa * 8) / 10, 0);
            lvgl_port_unlock();
            vTaskDelay(pdMS_TO_TICKS(70)); // Butter smooth frame rate
        }
        
        // Full Glow Hold at Peak
        vTaskDelay(pdMS_TO_TICKS(800));

        // Slow Subtle Dim
        for (int opa = 255; opa >= 90; opa -= 15) {
            lvgl_port_lock(0);
            lv_obj_set_style_text_opa(center_logo, opa, 0);
            lv_obj_set_style_text_opa(bottom_brand, (opa * 8) / 10, 0);
            lvgl_port_unlock();
            vTaskDelay(pdMS_TO_TICKS(70));
        }
        vTaskDelay(pdMS_TO_TICKS(350));
    }

    // 5. Final Full Bright Presentation (Holds for 2.2 seconds like a real device boot)
    lvgl_port_lock(0);
    lv_obj_set_style_text_opa(center_logo, LV_OPA_COVER, 0);
    lv_obj_set_style_text_opa(bottom_brand, LV_OPA_COVER, 0);
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(2200));

    // 6. Smooth Fade to Dark & Clean Switch to Eyes
    for (int opa = 255; opa >= 0; opa -= 30) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_logo, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    lvgl_port_lock(0);
    lv_obj_del(boot_screen);
    lvgl_port_unlock();

    display->SetEmotion("neutral");
}
