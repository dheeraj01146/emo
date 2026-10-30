#pragma once
#include "display.h"
#include <esp_lvgl_port.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

inline void RunEmoBootAnimation(Display* display) {
    if (!display) return;

    // 1. Fullscreen Pitch Black Canvas
    lvgl_port_lock(0);

    lv_obj_t* boot_screen = lv_obj_create(lv_layer_top());
    lv_obj_set_size(boot_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(boot_screen, 0, 0);
    lv_obj_set_style_radius(boot_screen, 0, 0);
    lv_obj_set_style_pad_all(boot_screen, 10, 0);
    lv_obj_clear_flag(boot_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Fast edex-ui / Cyber Terminal Label
    lv_obj_t* term_label = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(term_label, lv_color_hex(0x00FF66), 0); // Hacker Neon Green
    lv_obj_align(term_label, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(term_label, "");

    lvgl_port_unlock();

    // 2. edex-ui / Linux Boot Kernel Stream
    const char* kernel_logs[] = {
        "[  0.000000] Linux emo-core 5.15.0 #1 SMP PREEMPT\n",
        "[  0.041210] CPU: Xtensa dual-core ESP32-S3 @ 240MHz\n",
        "[  0.082400] Memory: 8192KB PSRAM Octal enabled [ OK ]\n",
        "[  0.134010] DMA: SPI Bounce buffers mounted [ OK ]\n",
        "[  0.198200] VFS: Loading file system partitions [ OK ]\n",
        "[  0.254100] net_core: scanning 802.11 b/g/n interfaces\n",
        "[  0.312000] wlan0: carrier connected to JIO SMART HOME\n",
        "[  0.370000] net_core: IPv4 address assigned [ OK ]\n",
        "[  0.428100] audio: I2S MAX98357A + INMP441 [ READY ]\n",
        "[  0.490000] systemd[1]: Reached target Graphical Interface.\n",
        "[  0.550000] emo_daemon: hardware handshake [ CONNECTED ]\n"
    };

    char log_stream[768] = "";
    for (int i = 0; i < 11; i++) {
        strcat(log_stream, kernel_logs[i]);
        lvgl_port_lock(0);
        lv_label_set_text(term_label, log_stream);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(110)); // Rapid stream effect
    }
    vTaskDelay(pdMS_TO_TICKS(800));

    // 3. Clear Screen for Huawei / HarmonyOS Style Reveal
    lvgl_port_lock(0);
    lv_obj_clean(boot_screen);

    // Glowing Halo / Horizon Arc Line
    lv_obj_t* horizon_glow = lv_obj_create(boot_screen);
    lv_obj_set_size(horizon_glow, 140, 4);
    lv_obj_set_style_bg_color(horizon_glow, lv_color_hex(0x00E5FF), 0); // Neon Cyan / Electric Blue
    lv_obj_set_style_radius(horizon_glow, 2, 0);
    lv_obj_set_style_border_width(horizon_glow, 0, 0);
    lv_obj_align(horizon_glow, LV_ALIGN_CENTER, 0, -32);
    lv_obj_set_style_bg_opa(horizon_glow, 0, 0);

    // Center Hero Logo
    lv_obj_t* center_title = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(center_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(center_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(center_title, "E M O\nI S   L I V E");
    lv_obj_align(center_title, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_style_text_opa(center_title, 0, 0);

    // Bottom Branding: powered by DHEERAJ
    lv_obj_t* bottom_brand = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(bottom_brand, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_align(bottom_brand, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(bottom_brand, "powered by\nD H E E R A J");
    lv_obj_align(bottom_brand, LV_ALIGN_BOTTOM_MID, 0, -18);
    lv_obj_set_style_text_opa(bottom_brand, 0, 0);

    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(300));

    // 4. Huawei Sunrise Horizon Expansion Animation
    for (int w = 20; w <= 160; w += 10) {
        lvgl_port_lock(0);
        lv_obj_set_width(horizon_glow, w);
        lv_obj_set_style_bg_opa(horizon_glow, LV_OPA_COVER, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }

    // 5. Smooth Text Fade-in & Horizon Pulse
    for (int opa = 20; opa <= 255; opa += 20) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_title, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, (opa * 8) / 10, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    // Full Presentation Hold
    vTaskDelay(pdMS_TO_TICKS(2500));

    // 6. Smooth Fade out to pitch black
    for (int opa = 255; opa >= 0; opa -= 30) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_title, opa, 0);
        lv_obj_set_style_text_opa(bottom_brand, opa, 0);
        lv_obj_set_style_bg_opa(horizon_glow, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(35));
    }

    // Cleanup & Return to Eyes
    lvgl_port_lock(0);
    lv_obj_del(boot_screen);
    lvgl_port_unlock();

    display->SetEmotion("neutral");
}
