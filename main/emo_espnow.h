#pragma once

#include "display.h"
#include <esp_log.h>
#include <string.h>

#define RELAY_TAG "EmoRelay"

enum EmoRelayDevice {
    DEVICE_LIGHT1 = 1, // GPIO 18
    DEVICE_LIGHT2 = 2, // GPIO 19
    DEVICE_FAN    = 3, // GPIO 21
    DEVICE_TV     = 4  // GPIO 22
};

typedef struct {
    uint8_t relay_num;
    uint8_t state;     // 1 = ON, 0 = OFF
} RelayCommand;

#if defined(CONFIG_IDF_TARGET_ESP32S3)
#include <esp_now.h>
#include <esp_wifi.h>

// Tumhare 4-relay board ka physical MAC Address
static const uint8_t RELAY_BOARD_MAC[6] = {0x68, 0x09, 0x47, 0x86, 0x26, 0xA0};

inline void InitEmoRelaySender() {
    esp_err_t err = esp_now_init();
    if (err != ESP_OK && err != ESP_ERR_ESPNOW_EXIST) {
        ESP_LOGE(RELAY_TAG, "ESP-NOW init failed: %d", err);
        return;
    }

    // Dynamic Wi-Fi Channel read karo jo Router ne EMO ko diya hai
    uint8_t current_channel = 0;
    wifi_second_chan_t second_chan;
    if (esp_wifi_get_channel(&current_channel, &second_chan) != ESP_OK || current_channel == 0) {
        current_channel = 6; // Safe default
    }

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, RELAY_BOARD_MAC, 6);
    peer.channel = current_channel; // <-- Ab Router ke channel se 100% match hoga!
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;

    // Purana peer clean karo agar pehle se registered ho
    if (esp_now_is_peer_exist(RELAY_BOARD_MAC)) {
        esp_now_del_peer(RELAY_BOARD_MAC);
    }

    if (esp_now_add_peer(&peer) == ESP_OK) {
        ESP_LOGI(RELAY_TAG, "Relay Board Paired dynamically on active Channel %d: 68:09:47:86:26:A0", current_channel);
    } else {
        ESP_LOGE(RELAY_TAG, "Failed to add Relay Board peer on Channel %d", current_channel);
    }
}

inline void SendRelayControl(uint8_t device_num, uint8_t state) {
    // Current channel check karke peer ko sync mein rakho
    uint8_t current_channel = 0;
    wifi_second_chan_t second_chan;
    esp_wifi_get_channel(&current_channel, &second_chan);

    if (!esp_now_is_peer_exist(RELAY_BOARD_MAC)) {
        InitEmoRelaySender();
    } else {
        // Agar router ne channel beech me badal diya ho, toh peer channel update karo
        esp_now_peer_info_t existing_peer = {};
        if (esp_now_get_peer(RELAY_BOARD_MAC, &existing_peer) == ESP_OK) {
            if (existing_peer.channel != current_channel && current_channel != 0) {
                existing_peer.channel = current_channel;
                esp_now_mod_peer(&existing_peer);
                ESP_LOGI(RELAY_TAG, "Updated Peer Channel to match Wi-Fi: %d", current_channel);
            }
        }
    }

    RelayCommand cmd = {device_num, state};
    esp_err_t result = esp_now_send(RELAY_BOARD_MAC, (const uint8_t*)&cmd, sizeof(RelayCommand));
    if (result == ESP_OK) {
        ESP_LOGI(RELAY_TAG, "Command Sent -> Device: %d, State: %d (Active Ch %d)", device_num, state, current_channel);
    } else {
        ESP_LOGE(RELAY_TAG, "Failed to send command: %d on Ch %d", result, current_channel);
    }
}
#else
inline void InitEmoRelaySender() {}
inline void SendRelayControl(uint8_t device_num, uint8_t state) {}
#endif
