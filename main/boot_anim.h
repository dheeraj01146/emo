error: 'lv_font_montserrat_28' was not declared in this scope; did you mean 'lv_font_montserrat_14'?
```[cite: 8]

Xiaozhi ke `sdkconfig` mein sirf **`lv_font_montserrat_14`** enabled hai, 28-size font disable rehta hai flash memory bachane ke liye[cite: 8].

Isko theek karne ke do simple solutions hain:
1. `lv_font_montserrat_14` use karein[cite: 8].
2. Font chhota na lage, iske liye LVGL ke text transform ya direct `lv_font_montserrat_14` ko clean set kar dein.

---

### Fixed `main/boot_anim.h`

Is code mein `lv_font_montserrat_28` ko hata kar standard `lv_font_montserrat_14` kar diya hai[cite: 8]:

```cpp
#pragma once

#include "display.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <lvgl.h>

#define BOOT_TAG "CinematicBoot"

inline void RunEmoBootAnimation(Display* display) {
    if (display == nullptr) return;

    ESP_LOGI(BOOT_TAG, "Running Cinematic Boot with expanding text animation...");

    // 1. Black canvas load karo
    lv_obj_t* boot_scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(boot_scr, lv_color_black(), 0);
    lv_scr_load(boot_scr);

    // 2. Center Expanding Ring (HarmonyOS glow style)
    lv_obj_t* ring = lv_arc_create(boot_scr);
    lv_obj_set_size(ring, 120, 120);
    lv_obj_center(ring);
    lv_arc_set_rotation(ring, 270);
    lv_arc_set_bg_angles(ring, 0, 360);
    lv_obj_remove_style(ring, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(ring, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_width(ring, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_color(ring, lv_color_hex(0x00E5FF), LV_PART_MAIN);

    // 3. Center EMO Logo (Using montserrat_14 standard enabled font)
    lv_obj_t* logo_label = lv_label_create(boot_scr);
    lv_label_set_text(logo_label, "E  M  O");
    lv_obj_set_style_text_color(logo_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(logo_label, &lv_font_montserrat_14, 0);
    lv_obj_align(logo_label, LV_ALIGN_CENTER, 0, -6);

    // 4. Neeche ka Dynamic Expanding Text Label
    lv_obj_t* sub_label = lv_label_create(boot_scr);
    lv_obj_set_style_text_color(sub_label, lv_color_hex(0x80D8FF), 0);
    lv_obj_set_style_text_font(sub_label, &lv_font_montserrat_14, 0);
    lv_obj_align(sub_label, LV_ALIGN_BOTTOM_MID, 0, -20);

    // Stage A: Ring ka smooth 360 degree sweep
    for (int i = 0; i <= 360; i += 20) {
        lv_arc_set_angles(ring, 0, i);
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    // Stage B: Pehle center me sirf "by" aayega
    lv_label_set_text(sub_label, "by");
    lv_obj_align(sub_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    vTaskDelay(pdMS_TO_TICKS(400));

    // Stage C: "by" me se akshar expand hokar "powered by dheeraj" banenge
    const char* text_frames[] = {
        "d by d",
        "ed by dh",
        "red by dhee",
        "ered by dheer",
        "wered by dheera",
        "owered by dheeraj",
        "powered by dheeraj"
    };

    for (const char* frame : text_frames) {
        lv_label_set_text(sub_label, frame);
        lv_obj_align(sub_label, LV_ALIGN_BOTTOM_MID, 0, -20);
        vTaskDelay(pdMS_TO_TICKS(90));
    }

    // Stage D: Logo aur final text ka bold glow hold
    lv_obj_set_style_text_color(logo_label, lv_color_hex(0x00E5FF), 0);
    lv_obj_set_style_text_color(sub_label, lv_color_white(), 0);
    vTaskDelay(pdMS_TO_TICKS(1200));

    // Stage E: Clean wipe aur EMO eyes load
    lv_obj_del(boot_scr);

    if (display != nullptr) {
        display->SetStatus("EMO ONLINE");
        display->SetEmotion("happy");
    }
}
