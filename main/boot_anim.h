#pragma once
#include "display.h"
#include <esp_lvgl_port.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

inline void RunEmoBootAnimation(Display* display) {
    if (!display) return;

    // 1. Fullscreen Base Canvas
    lvgl_port_lock(0);

    lv_obj_t* boot_screen = lv_obj_create(lv_layer_top());
    lv_obj_set_size(boot_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(boot_screen, 0, 0);
    lv_obj_set_style_radius(boot_screen, 0, 0);
    lv_obj_set_style_pad_all(boot_screen, 12, 0);
    lv_obj_clear_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Green Cyber Terminal
    lv_obj_t* term_label = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(term_label, lv_color_hex(0x00FF66), 0);
    lv_obj_align(term_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(term_label, "");

    lvgl_port_unlock();

    // 2. Diagnostics Logs (Aaram se padhne ka pace)
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
        vTaskDelay(pdMS_TO_TICKS(400));
    }
    vTaskDelay(pdMS_TO_TICKS(1200));

    // 3. Clear Screen & Suspense Hold
    lvgl_port_lock(0);
    lv_obj_clean(boot_screen);
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(800));

    // 4. Oppo Left & Right Sliding Bars Setup
    lvgl_port_lock(0);

    lv_obj_t* left_wing = lv_obj_create(boot_screen);
    lv_obj_set_size(left_wing, 4, 70);
    lv_obj_set_style_bg_color(left_wing, lv_color_hex(0x00E5FF), 0); // Neon Cyan
    lv_obj_set_style_radius(left_wing, 2, 0);
    lv_obj_set_style_border_width(left_wing, 0, 0);
    lv_obj_align(left_wing, LV_ALIGN_CENTER, -120, -20); // Left side off-center

    lv_obj_t* right_wing = lv_obj_create(boot_screen);
    lv_obj_set_size(right_wing, 4, 70);
    lv_obj_set_style_bg_color(right_wing, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_radius(right_wing, 2, 0);
    lv_obj_set_style_border_width(right_wing, 0, 0);
    lv_obj_align(right_wing, LV_ALIGN_CENTER, 120, -20); // Right side off-center

    // Central Oppo Frame Container
    lv_obj_t* oppo_window = lv_obj_create(boot_screen);
    lv_obj_set_size(oppo_window, 10, 70);
    lv_obj_align(oppo_window, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_bg_color(oppo_window, lv_color_hex(0x0C0C14), 0);
    lv_obj_set_style_bg_opa(oppo_window, 0, 0);
    lv_obj_set_style_border_width(oppo_window, 0, 0);
    lv_obj_set_style_radius(oppo_window, 8, 0);
    lv_obj_clear_flag(oppo_window, LV_OBJ_FLAG_SCROLLABLE);

    // EMO AI ROBOT Logo
    lv_obj_t* center_logo = lv_label_create(oppo_window);
    lv_obj_set_style_text_color(center_logo, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(center_logo, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(center_logo, "EMO\nAI ROBOT");
    lv_obj_align(center_logo, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_opa(center_logo, 0, 0);

    // powered by DHEERAJ
    lv_obj_t* bottom_brand = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(bottom_brand, lv_color_hex(0x999999), 0);
    lv_obj_set_style_text_align(bottom_brand, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(bottom_brand, "powered by\nD H E E R A J");
    lv_obj_align(bottom_brand, LV_ALIGN_BOTTOM_MID, 0, -18);
    lv_obj_set_style_text_opa(bottom_brand, 0, 0);

    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(300));

    // 5. Oppo Left-Right Move & Dock Animation
    // Left & Right lines slide in towards the center
    for (int offset = 120; offset >= 10; offset -= 8) {
        lvgl_port_lock(0);
        lv_obj_align(left_wing, LV_ALIGN_CENTER, -offset, -20);
        lv_obj_align(right_wing, LV_ALIGN_CENTER, offset, -20);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }

    // 6. Window Open Expansion (Center se dono taraf parde ki tarah khulna)
    for (int w = 10; w <= 180; w += 10) {
        lvgl_port_lock(0);
        lv_obj_set_size(oppo_window, w, 76);
        lv_obj_set_style_bg_opa(oppo_window, LV_OPA_COVER, 0);
        // Wings slide outwards pushing the window boundaries
        lv_obj_align(left_wing, LV_ALIGN_CENTER, -(w / 2), -20);
        lv_obj_align(right_wing, LV_ALIGN_CENTER, (w / 2), -20);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(30));
    }

    // Border bars fade into window edges smoothly
    lvgl_port_lock(0);
    lv_obj_del(left_wing);
    lv_obj_del(right_wing);
    lv_obj_set_style_border_color(oppo_window, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(oppo_window, 2, 0);
    lvgl_port_unlock();

    // 7. Smooth Slow Fade-in of Logo & Powered By Text
    for (int opa = 20; opa <= 255; opa += 15) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_logo, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, (opa * 8) / 10, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(60));
    }

    // 8. Steady Rock Freeze (3.5 Seconds hold)
    vTaskDelay(pdMS_TO_TICKS(3500));

    // 9. Graceful Curtain Close & Fade Out
    for (int opa = 255; opa >= 0; opa -= 25) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_logo, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, opa, 0);
        lv_obj_set_style_bg_opa(oppo_window, opa, 0);
        lv_obj_set_style_border_opa(oppo_window, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    // 10. Cleanup & Switch to EMO
    lvgl_port_lock(0);
    lv_obj_del(boot_screen);
    lvgl_port_unlock();

    display->SetEmotion("neutral");
}
