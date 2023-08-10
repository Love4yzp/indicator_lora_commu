#ifndef _FRAME_H
#define _FRAME_H
#include <Arduino.h>
#include <vector>

/**
 * |Header | Data payload | CRC |
 */

#pragma pack(1)
enum topics {
    TOPICS_MIN   = 0x00,
    TOPICS_SEN5x = 0x01,
    // TOPICS_MOISTURE,
    TOPIC_MAX,
};

#pragma pack(1)
typedef struct
{
    enum topics topic;         /*msg type*/
    uint8_t dataLength;
    std::vector<uint8_t> data; /*actual data of payload*/
    uint16_t crc;
} Frame_t;

String packFrame(Frame_t frame);
void deleteFrame(Frame_t *frame);

uint16_t crc16_ccitt(const uint8_t *data, size_t length);

#endif
