#pragma once
#include "display.h"
#include <esp_lvgl_port.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

inline void RunEmoBootAnimation(Display* display) {
    if (!display) return;

    // 1. Fullscreen Base Container (Pitch Black for Terminal)
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

    // 2. Heavy & Relaxed Terminal Diagnostics Stream (No Rush)
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
        vTaskDelay(pdMS_TO_TICKS(480)); // Ek-ek line ko aaram se aane do (~5 sec total)
    }

    // Saare logs aane ke baad 2 second aaram se padhne ka pause
    vTaskDelay(pdMS_TO_TICKS(2000));

    // 3. Clear Screen & Suspense Pause in Pitch Black
    lvgl_port_lock(0);
    lv_obj_clean(boot_screen);
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x000000), 0);
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second silent dark wait

    // 4. Setup Honor Cyan Liquid Theme Objects
    lvgl_port_lock(0);
    // Vibrant Honor 7C Cyan Blue (#00A8FF)
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x00A8FF), 0);
    lv_obj_set_style_bg_opa(boot_screen, LV_OPA_COVER, 0);

    // Liquid Gel Droplet Pill
    lv_obj_t* liquid_bubble = lv_obj_create(boot_screen);
    lv_obj_set_size(liquid_bubble, 20, 20);
    lv_obj_align(liquid_bubble, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_bg_color(liquid_bubble, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_radius(liquid_bubble, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(liquid_bubble, 0, 0);
    lv_obj_set_style_bg_opa(liquid_bubble, 0, 0);

    // Center Text: EMO IS READY
    lv_obj_t* center_text = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(center_text, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(center_text, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(center_text, "EMO\nIS READY");
    lv_obj_align(center_text, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_text_opa(center_text, 0, 0);

    // Bottom Branding: powered by DHEERAJ
    lv_obj_t* brand_text = lv_label_create(boot_screen);
    lv_obj_set_style_text_color(brand_text, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_align(brand_text, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(brand_text, "powered by\nD H E E R A J");
    lv_obj_align(brand_text, LV_ALIGN_BOTTOM_MID, 0, -18);
    lv_obj_set_style_text_opa(brand_text, 0, 0);

    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(500));

    // 5. Ultra Smooth Liquid Bubble Swell (Slow Motion Gel Rise)
    for (int size = 20; size <= 150; size += 5) {
        lvgl_port_lock(0);
        lv_obj_set_size(liquid_bubble, size + 40, (size * 6) / 10 + 10);
        lv_obj_set_style_bg_opa(liquid_bubble, 40 + (size * 140 / 150), 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(65)); // Butter smooth slow expansion (~1.7 sec)
    }

    // 6. Calm Text Fade-In & Pop
    for (int opa = 15; opa <= 255; opa += 15) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_text, opa, 0);
        lv_obj_set_style_text_opa(brand_text, (opa * 9) / 10, 0);
        lv_obj_set_style_bg_opa(liquid_bubble, 255 - (opa * 180 / 255), 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(70)); // Slow elegant text bloom (~1.2 sec)
    }

    // 7. Solid Rock Freeze Hold (4 Pure Seconds)
    vTaskDelay(pdMS_TO_TICKS(4000));

    // 8. Gentle Fade Out to Black
    for (int opa = 255; opa >= 0; opa -= 15) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_opa(center_text, opa, 0);
        lv_obj_set_style_text_opa(brand_text, opa, 0);
        lv_obj_set_style_bg_opa(boot_screen, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(55));
    }

    // 9. Clean Exit & Switch to EMO
    lvgl_port_lock(0);
    lv_obj_del(boot_screen);
    lvgl_port_unlock();

    display->SetEmotion("neutral");
}
