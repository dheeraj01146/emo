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

    // 2. Slow Diagnostics Stream
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
        vTaskDelay(pdMS_TO_TICKS(380));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 3. Clear to Black & Suspense Pause
    lvgl_port_lock(0);
    lv_obj_clean(boot_screen);
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(600));

    // 4. Center EMO AI ROBOT Box & Branding
    lvgl_port_lock(0);

    lv_obj_t* oppo_window = lv_obj_create(boot_screen);
    lv_obj_set_size(oppo_window, 180, 76);
    lv_obj_align(oppo_window, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_bg_color(oppo_window, lv_color_hex(0x0C0C14), 0);
    lv_obj_set_style_bg_opa(oppo_window, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(oppo_window, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(oppo_window, 2, 0);
    lv_obj_set_style_radius(oppo_window, 8, 0);
    lv_obj_clear_flag(oppo_window, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* center_logo = lv_label_create(oppo_window);
    lv_obj_set_style_text_color(center_logo, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(center_logo, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(center_logo, "EMO\nAI ROBOT");
    lv_obj_align(center_logo, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* bottom_brand = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(bottom_brand, lv_color_hex(0x999999), 0);
    lv_obj_set_style_text_align(bottom_brand, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(bottom_brand, "powered by\nD H E E R A J");
    lv_obj_align(bottom_brand, LV_ALIGN_BOTTOM_MID, 0, -18);

    lvgl_port_unlock();

    // Box Hold (2.5 Seconds steady)
    vTaskDelay(pdMS_TO_TICKS(2500));

    // Fade out logo cleanly to pure black
    for (int opa = 255; opa >= 0; opa -= 25) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_logo, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, opa, 0);
        lv_obj_set_style_bg_opa(oppo_window, opa, 0);
        lv_obj_set_style_border_opa(oppo_window, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(30));
    }

    lvgl_port_lock(0);
    lv_obj_clean(boot_screen); // Complete pitch black screen
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(500)); // Dark pause

    // 5. OPPO F11 PRO STYLE WINDOW SLIT UNLOCK ANIMATION
    lvgl_port_lock(0);
    // Glowing horizontal white line/slit in center
    lv_obj_t* light_slit = lv_obj_create(boot_screen);
    lv_obj_set_size(light_slit, 10, 3);
    lv_obj_align(light_slit, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(light_slit, lv_color_hex(0xFFFFFF), 0); // Pure White Light
    lv_obj_set_style_bg_opa(light_slit, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(light_slit, 0, 0);
    lv_obj_set_style_radius(light_slit, 2, 0);
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(200));

    // Phase A: Slit expands LEFT & RIGHT across the screen
    for (int w = 10; w <= LV_HOR_RES; w += 16) {
        lvgl_port_lock(0);
        lv_obj_set_size(light_slit, w, 3);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // Phase B: Slit expands VERTICALLY (Window Unfolding / Curtain opening)
    for (int h = 3; h <= LV_VER_RES; h += 18) {
        lvgl_port_lock(0);
        lv_obj_set_size(light_slit, LV_HOR_RES, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(18));
    }

    // Phase C: Bright Window Dissolve to reveal EMO face
    for (int opa = 255; opa >= 0; opa -= 30) {
        lvgl_port_lock(0);
        lv_obj_set_style_bg_opa(light_slit, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }

    // 6. Cleanup
    lvgl_port_lock(0);
    lv_obj_del(boot_screen);
    lvgl_port_unlock();

    display->SetEmotion("neutral");
}
