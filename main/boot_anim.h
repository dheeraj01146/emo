#pragma once
#include "display.h"
#include <esp_lvgl_port.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

inline void RunEmoBootAnimation(Display* display) {
    if (!display) return;

    // 1. Fullscreen Base Canvas (Pitch Black)
    lvgl_port_lock(0);

    lv_obj_t* boot_screen = lv_obj_create(lv_layer_top());
    lv_obj_set_size(boot_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(boot_screen, 0, 0);
    lv_obj_set_style_radius(boot_screen, 0, 0);
    lv_obj_set_style_pad_all(boot_screen, 12, 0);
    lv_obj_clear_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Green Cyber Terminal Label
    lv_obj_t* term_label = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(term_label, lv_color_hex(0x00FF66), 0);
    lv_obj_align(term_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(term_label, "");

    lvgl_port_unlock();

    // 2. Slow & Relaxed Terminal Checks (No Rush)
    const char* kernel_logs[] = {
        "> INITIALIZING EMO CORE...\n",
        "> RAM [8MB PSRAM] ........ [ OK ]\n",
        "> DISPLAY SUBSYSTEM ...... [ OK ]\n",
        "> ENGINE SIMULATOR ....... [ OK ]\n",
        "> HARDWARE SERVO ......... [ OK ]\n",
        "> NETWORK (WIFI) ......... [ CONNECTED ]\n"
    };

    char log_stream[512] = "";
    for (int i = 0; i < 6; i++) {
        strcat(log_stream, kernel_logs[i]);
        lvgl_port_lock(0);
        lv_label_set_text(term_label, log_stream);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(420)); // Slow readable pace
    }
    vTaskDelay(pdMS_TO_TICKS(1400)); // 1.4 sec hold

    // 3. Clear Screen & Suspense Pause
    lvgl_port_lock(0);
    lv_obj_clean(boot_screen);
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(900));

    // 4. Setup Oppo Style Window Container & Text Elements
    lvgl_port_lock(0);

    // Oppo Window Box (Starts as a thin center line/slit)
    lv_obj_t* oppo_window = lv_obj_create(boot_screen);
    lv_obj_set_size(oppo_window, 20, 2);
    lv_obj_align(oppo_window, LV_ALIGN_CENTER, 0, -25);
    lv_obj_set_style_bg_color(oppo_window, lv_color_hex(0x111118), 0); // Premium dark graphite
    lv_obj_set_style_bg_opa(oppo_window, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(oppo_window, lv_color_hex(0x00E5FF), 0); // Neon cyan border
    lv_obj_set_style_border_width(oppo_window, 2, 0);
    lv_obj_set_style_radius(oppo_window, 8, 0);
    lv_obj_clear_flag(oppo_window, LV_OBJ_FLAG_SCROLLABLE);

    // Center Big Logo: EMO AI ROBOT
    lv_obj_t* center_logo = lv_label_create(oppo_window);
    lv_obj_set_style_text_color(center_logo, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(center_logo, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(center_logo, "EMO\nAI ROBOT");
    lv_obj_align(center_logo, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_opa(center_logo, 0, 0);

    // Bottom Branding: powered by DHEERAJ
    lv_obj_t* bottom_brand = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(bottom_brand, lv_color_hex(0x999999), 0);
    lv_obj_set_style_text_align(bottom_brand, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(bottom_brand, "powered by\nD H E E R A J");
    lv_obj_align(bottom_brand, LV_ALIGN_BOTTOM_MID, 0, -18);
    lv_obj_set_style_text_opa(bottom_brand, 0, 0);

    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(400));

    // 5. Oppo Window Opening Animation (Horizontal Slit expand -> Vertical Window Open)
    // Step A: Width Expansion (Horizontal Slit opens)
    for (int w = 20; w <= 180; w += 10) {
        lvgl_port_lock(0);
        lv_obj_set_size(oppo_window, w, 3);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(30));
    }

    // Step B: Height Expansion (Window unfolds vertically)
    for (int h = 3; h <= 95; h += 6) {
        lvgl_port_lock(0);
        lv_obj_set_size(oppo_window, 180, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(35));
    }

    // 6. Smooth Reveal of Center Text & Bottom Branding
    for (int opa = 15; opa <= 255; opa += 15) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_logo, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, (opa * 8) / 10, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(60)); // Butter smooth slow reveal
    }

    // 7. Solid Freeze Hold (Aaram se 3.5 Seconds freeze taaki sir aur sab dekh sakein)
    vTaskDelay(pdMS_TO_TICKS(3500));

    // 8. Elegant Window Close / Fade Out
    for (int opa = 255; opa >= 0; opa -= 20) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_logo, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, opa, 0);
        lv_obj_set_style_bg_opa(oppo_window, opa, 0);
        lv_obj_set_style_border_opa(oppo_window, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    // 9. Cleanup & Return to Neutral Robot Eyes
    lvgl_port_lock(0);
    lv_obj_del(boot_screen);
    lvgl_port_unlock();

    display->SetEmotion("neutral");
}
