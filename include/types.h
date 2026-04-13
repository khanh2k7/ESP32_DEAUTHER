#ifndef TYPES_H
#define TYPES_H

typedef struct __attribute__((packed)) {
    uint8_t frame_control[2] = {0xC0, 0x00};
    uint8_t duration[2] = {0x00, 0x00};
    uint8_t station[6];
    uint8_t sender[6];
    uint8_t access_point[6];
    uint8_t fragment_sequence[2] = {0xF0, 0xFF};
    uint16_t reason;
} deauth_frame_t;

typedef struct __attribute__((packed)) {
    uint16_t frame_ctrl;
    uint16_t duration;
    uint8_t dest[6];
    uint8_t src[6];
    uint8_t bssid[6];
    uint16_t sequence_ctrl;
} mac_hdr_t;   // rút gọn addr4

typedef struct {
    mac_hdr_t hdr;
    uint8_t payload[0];
} wifi_packet_t;

const wifi_promiscuous_filter_t filt = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT
};

#endif