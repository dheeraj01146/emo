#pragma once

#include "display.h"
#include <esp_log.h>
#include <cstdio>

#define BOOT_TAG "CyberBoot"

#if defined(CONFIG_DISPLAY_TYPE) || defined(CONFIG_LV_COLOR_DEPTH)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>
#include <esp_lvgl_port.h>

/* Helper: clean borderless object */
inline lv_obj_t* BootObj(lv_obj_t* parent, lv_coord_t w, lv_coord_t h,
                         uint32_t bg_hex, lv_coord_t radius) {
    lv_obj_t* o = lv_obj_create(parent);
    lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, lv_color_hex(bg_hex), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_set_scrollbar_mode(o, LV_SCROLLBAR_MODE_OFF);
    return o;
}

/* Helper: dono eyes ek saath control (dx, dy = offset, h = height) */
inline void SetEyes(lv_obj_t* el, lv_obj_t* er, int dx, int dy, int h) {
    lv_obj_set_size(el, 46, h);
    lv_obj_align(el, LV_ALIGN_CENTER, -30 + dx, -15 + dy);
    lv_obj_set_size(er, 46, h);
    lv_obj_align(er, LV_ALIGN_CENTER, 30 + dx, -15 + dy);
}

inline void RunEmoBootAnimation(Display* display) {
    if (display == nullptr) return;

    ESP_LOGI(BOOT_TAG, "Running EMO WAKE-UP v2 — ALIVE EYES!");

    lv_obj_t* prev_scr = lv_scr_act();

    /* ---------- PHASE 0: BLACK CANVAS ---------- */
    lvgl_port_lock(0);
    lv_obj_t* boot_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(boot_scr, lv_color_black(), 0);
    lv_scr_load(boot_scr);
    lvgl_port_unlock();
    vTaskDelay(pdMS_TO_TICKS(400));

    /* ---------- PHASE 1: POWER SPARK FLICKER ---------- */
    lvgl_port_lock(0);
    lv_obj_t* spark = BootObj(boot_scr, 70, 2, 0x00E5FF, 1);
    lv_obj_align(spark, LV_ALIGN_CENTER, 0, 0);
    lvgl_port_unlock();

    for (int i = 0; i < 3; i++) {
        lvgl_port_lock(0);
        lv_obj_add_flag(spark, LV_OBJ_FLAG_HIDDEN);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(60));
        lvgl_port_lock(0);
        lv_obj_clear_flag(spark, LV_OBJ_FLAG_HIDDEN);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(90));
    }
    lvgl_port_lock(0);
    lv_obj_del(spark);
    lvgl_port_unlock();

    /* ---------- PHASE 2: EYES WAKE-UP PERFORMANCE ---------- */
    lvgl_port_lock(0);
    lv_obj_t* eye_l = BootObj(boot_scr, 46, 2, 0x00E5FF, LV_RADIUS_CIRCLE);
    lv_obj_t* eye_r = BootObj(boot_scr, 46, 2, 0x00E5FF, LV_RADIUS_CIRCLE);
    SetEyes(eye_l, eye_r, 0, 0, 2);
    lvgl_port_unlock();

    // (a) Eyes khul rahi hain
    for (int h = 2; h <= 46; h += 4) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 0, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    vTaskDelay(pdMS_TO_TICKS(300));

    // (b) Ek normal blink (dono saath mein)
    for (int h = 46; h >= 4; h -= 7) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 0, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    for (int h = 4; h <= 46; h += 7) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 0, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelay(pdMS_TO_TICKS(250));

    // (c) Curious glance — LEFT... RIGHT... (nayi duniya dekh raha hai 👀)
    for (int dx = 0; dx >= -10; dx -= 2) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, dx, 0, 46);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    vTaskDelay(pdMS_TO_TICKS(280));
    for (int dx = -10; dx <= 10; dx += 2) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, dx, 0, 46);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    vTaskDelay(pdMS_TO_TICKS(280));
    for (int dx = 10; dx >= 0; dx -= 2) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, dx, 0, 46);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    vTaskDelay(pdMS_TO_TICKS(200));

    // (d) THE WINK — right eye AKELI blink karegi!
    for (int h = 46; h >= 2; h -= 4) {
        lvgl_port_lock(0);
        lv_obj_set_size(eye_r, 46, h);
        lv_obj_align(eye_r, LV_ALIGN_CENTER, 30, -15);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    for (int h = 2; h <= 46; h += 4) {
        lvgl_port_lock(0);
        lv_obj_set_size(eye_r, 46, h);
        lv_obj_align(eye_r, LV_ALIGN_CENTER, 30, -15);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(25));
    }
    vTaskDelay(pdMS_TO_TICKS(250));

    // (e) Happy squint — dono eyes smile style ^ ^
    for (int h = 46; h >= 10; h -= 9) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 0, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    vTaskDelay(pdMS_TO_TICKS(150));
    for (int h = 10; h <= 46; h += 9) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 0, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    /* ---------- PHASE 3: RADAR PING + EYE BREATHING ---------- */
    lvgl_port_lock(0);
    lv_obj_t* ring1 = lv_obj_create(boot_scr);
    lv_obj_set_size(ring1, 10, 10);
    lv_obj_align(ring1, LV_ALIGN_CENTER, 0, -15);
    lv_obj_set_style_bg_opa(ring1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(ring1, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(ring1, 2, 0);
    lv_obj_set_style_radius(ring1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_scrollbar_mode(ring1, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t* ring2 = lv_obj_create(boot_scr);
    lv_obj_set_size(ring2, 10, 10);
    lv_obj_align(ring2, LV_ALIGN_CENTER, 0, -15);
    lv_obj_set_style_bg_opa(ring2, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_color(ring2, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_border_width(ring2, 2, 0);
    lv_obj_set_style_radius(ring2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_scrollbar_mode(ring2, LV_SCROLLBAR_MODE_OFF);
    lvgl_port_unlock();

    for (int s = 10; s <= 150; s += 10) {
        lvgl_port_lock(0);
        lv_obj_set_size(ring1, s, s);
        lv_obj_align(ring1, LV_ALIGN_CENTER, 0, -15);
        lv_obj_set_style_border_opa(ring1, 255 - (s * 255 / 150), 0);

        int s2 = s - 40; if (s2 < 10) s2 = 10;
        lv_obj_set_size(ring2, s2, s2);
        lv_obj_align(ring2, LV_ALIGN_CENTER, 0, -15);
        lv_obj_set_style_border_opa(ring2, 255 - (s2 * 255 / 150), 0);

        // Eyes gently breathe — freeze nahi lagega
        SetEyes(eye_l, eye_r, 0, 0, ((s / 10) % 2) ? 42 : 46);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(30));
    }
    lvgl_port_lock(0);
    lv_obj_del(ring1);
    lv_obj_del(ring2);
    lvgl_port_unlock();

    /* ---------- PHASE 4: TYPING "EMO" — EYES READING ---------- */
    lvgl_port_lock(0);
    lv_obj_t* emo_txt = lv_label_create(boot_scr);
    lv_label_set_text(emo_txt, "");
    lv_obj_set_style_text_color(emo_txt, lv_color_hex(0x00E5FF), 0);
    lv_obj_align(emo_txt, LV_ALIGN_CENTER, 0, 45);
    lvgl_port_unlock();

    char buf[20];
    const char* name = "EMO";
    const int read_dx[3] = {-6, 0, 6};   // eyes letters ko follow karein
    for (int i = 1; i <= 3; i++) {
        snprintf(buf, sizeof(buf), "%.*s_", i, name);
        lvgl_port_lock(0);
        lv_label_set_text(emo_txt, buf);
        lv_obj_align(emo_txt, LV_ALIGN_CENTER, 0, 45);
        SetEyes(eye_l, eye_r, read_dx[i - 1], 8, 46);  // neeche dekh raha hai
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(150));
    }
    // Cursor blink
    for (int b = 0; b < 2; b++) {
        lvgl_port_lock(0);
        lv_label_set_text(emo_txt, "EMO");
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(120));
        lvgl_port_lock(0);
        lv_label_set_text(emo_txt, "EMO_");
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(120));
    }
    // GLITCH — eyes bhi shake karein
    for (int g = 0; g < 6; g++) {
        lvgl_port_lock(0);
        lv_obj_set_style_text_color(emo_txt, (g % 2) ? lv_color_white() : lv_color_hex(0xFF2255), 0);
        lv_obj_align(emo_txt, LV_ALIGN_CENTER, (g % 2) ? 3 : -3, 45);
        SetEyes(eye_l, eye_r, (g % 2) ? 3 : -3, 8, 46);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(45));
    }
    lvgl_port_lock(0);
    lv_label_set_text(emo_txt, "EMO");
    lv_obj_set_style_text_color(emo_txt, lv_color_hex(0x00E5FF), 0);
    lv_obj_align(emo_txt, LV_ALIGN_CENTER, 0, 45);
    SetEyes(eye_l, eye_r, 0, 8, 46);
    lvgl_port_unlock();

    /* ---------- PHASE 5: SUBTITLE + AUTHOR (TOP-RIGHT FIX) ---------- */
    lvgl_port_lock(0);
    lv_obj_t* sub = lv_label_create(boot_scr);
    lv_label_set_text(sub, "AI ROBOT");
    lv_obj_set_style_text_color(sub, lv_color_white(), 0);
    lv_obj_align(sub, LV_ALIGN_CENTER, 0, 68);

    lv_obj_t* author = lv_label_create(boot_scr);
    lv_label_set_text(author, "POWERED BY DHEERAJ");
    lv_obj_set_style_text_color(author, lv_color_hex(0x00E5FF), 0);
    lv_obj_align(author, LV_ALIGN_TOP_RIGHT, -10, 8);
    lvgl_port_unlock();

    /* ---------- PHASE 6: LOADING BAR — EYES FOLLOW KARTI HAIN ---------- */
    lvgl_port_lock(0);
    lv_obj_t* bar_bg = BootObj(boot_scr, 180, 8, 0x0A1A2A, 4);
    lv_obj_align(bar_bg, LV_ALIGN_CENTER, 0, 92);
    lv_obj_t* bar_fill = BootObj(bar_bg, 2, 8, 0x00E5FF, 4);
    lv_obj_align(bar_fill, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t* pct_lbl = lv_label_create(boot_scr);
    lv_label_set_text(pct_lbl, "BOOTING 0%");
    lv_obj_set_style_text_color(pct_lbl, lv_color_hex(0x8899AA), 0);
    lv_obj_align(pct_lbl, LV_ALIGN_CENTER, 0, 110);
    lvgl_port_unlock();

    for (int p = 0; p <= 100; p += 5) {
        lvgl_port_lock(0);
        lv_obj_set_width(bar_fill, 2 + (176 * p) / 100);
        snprintf(buf, sizeof(buf), "BOOTING %d%%", p);
        lv_label_set_text(pct_lbl, buf);
        lv_obj_align(pct_lbl, LV_ALIGN_CENTER, 0, 110);
        SetEyes(eye_l, eye_r, (p - 50) / 4, 8, 46);   // eyes bar track karti hain!
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(35));
    }

    // Hold ke dauran ek slow satisfied blink
    for (int h = 46; h >= 6; h -= 8) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 8, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(40));
    }
    for (int h = 6; h <= 46; h += 8) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 8, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    /* ---------- PHASE 7: FINAL SMILE + WHITE FLASH ---------- */
    // Flash se pehle dono eyes happy squint — "smile" 😊
    for (int h = 46; h >= 10; h -= 9) {
        lvgl_port_lock(0);
        SetEyes(eye_l, eye_r, 0, 8, h);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    lvgl_port_lock(0);
    lv_obj_t* flash = lv_obj_create(boot_scr);
    lv_obj_set_size(flash, 320, 240);
    lv_obj_align(flash, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(flash, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(flash, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(flash, 0, 0);
    lv_obj_set_scrollbar_mode(flash, LV_SCROLLBAR_MODE_OFF);
    lvgl_port_unlock();

    for (int opa = 0; opa <= 255; opa += 51) {
        lvgl_port_lock(0);
        lv_obj_set_style_bg_opa(flash, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(15));
    }
    for (int opa = 255; opa >= 0; opa -= 51) {
        lvgl_port_lock(0);
        lv_obj_set_style_bg_opa(flash, opa, 0);
        lvgl_port_unlock();
        vTaskDelay(pdMS_TO_TICKS(15));
    }

    /* ---------- CLEAN HANDOVER ---------- */
    lvgl_port_lock(0);
    if (prev_scr != nullptr) {
        lv_scr_load(prev_scr);
    }
    lv_obj_del(boot_scr);
    lvgl_port_unlock();

    vTaskDelay(pdMS_TO_TICKS(100));

    display->SetEmotion("neutral");
    display->SetStatus("EMO ONLINE");
}
#else
inline void RunEmoBootAnimation(Display* display) {}
#endif
