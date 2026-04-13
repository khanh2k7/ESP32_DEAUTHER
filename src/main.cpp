#include <WiFi.h>
#include <esp_wifi.h>
#include "definitions.h"
#include "web_interface.h"
#include "deauth.h"

int curr_channel = 1;

void setup() {
#ifdef SERIAL_DEBUG
    Serial.begin(115200);
    delay(1000);
    DEBUG_PRINTLN("ESP32 Deauther started!");
#endif

#ifdef LED
    pinMode(LED, OUTPUT);
#endif

    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(AP_SSID, AP_PASS);

    start_web_interface();
}

void loop() {
    web_interface_handle_client();

    if (deauth_type == DEAUTH_TYPE_ALL) {
        if (curr_channel > CHANNEL_MAX) curr_channel = 1;
        esp_wifi_set_channel(curr_channel, WIFI_SECOND_CHAN_NONE);
        curr_channel++;
        delay(8);   // hop nhanh hơn một chút
    }
}