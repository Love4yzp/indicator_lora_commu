#include "frame.h"

// #define DEBUG
#include "esp_log.h"
Frame_t *parsePayload(uint8_t *payload, uint8_t length) {
    Frame_t *frame = (Frame_t *)malloc(sizeof(Frame_t));
    if (frame == NULL) {
        ESP_LOGE("parsePayload", "Failed to allocate memory for frame");
        return NULL;
    }

    frame->topic = (enum topics)payload[0];
    ESP_LOGW("parsePayload", "topic: %d", frame->topic);
    frame->dataLength = payload[1];
    ESP_LOGW("parsePayload", "dataLength: %d", frame->dataLength);
    frame->data = (uint8_t *)malloc(frame->dataLength);
    if (frame->data == NULL) {
        ESP_LOGE("parsePayload", "Failed to allocate memory for frame data");
        free(frame); // Clean up previously allocated memory
        return NULL;
    }

    memcpy(frame->data, payload + 2, frame->dataLength);
    for (int i = 0; i < frame->dataLength; i++) {
        ESP_LOGW("parsePayload", "payload[%d]: %02X", i, payload[i + 2]);
    }
    frame->crc = (uint16_t)payload[length - 2] << 8 | (uint16_t)payload[length - 1];

    if (frame->crc != crc16_ccitt(payload, length - 2)) {
        ESP_LOGE("parsePayload", "CRC mismatch");
        free(frame->data);
        free(frame);
        return NULL;
    }

    return frame;
}

// Frame_t *parsePayload(Frame_t *frame, uint8_t *payload, uint8_t length) {
//     frame    = (Frame_t *)malloc(sizeof(Frame_t));
//     frame->topic      = (enum topics)payload[0];
//     ESP_LOGW("parsePayload", "topic: %d", frame->topic);
//     frame->dataLength = payload[1];
//     ESP_LOGW("parsePayload", "dataLength: %d", frame->dataLength);
//     frame->data       = (uint8_t *)malloc(frame->dataLength);
//     memcpy(frame->data, payload + 2, frame->dataLength);
//     for(int i = 0; i < frame->dataLength; i++) {
//         ESP_LOGW("parsePayload", "payload[%d]: %02X", i, payload[i + 2]);
//     }
//     frame->crc = (uint16_t)payload[length - 2] << 8 | (uint16_t)payload[length - 1];
//     return frame;
// }

void deleteFrame(Frame_t *frame) {
    free(frame->data);
    free(frame);
}

uint16_t crc16_ccitt(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint8_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc & 0xFFFF;
}
