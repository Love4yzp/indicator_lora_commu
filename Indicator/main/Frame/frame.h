#ifndef __SIMPLE_FRAME_H
#define __SIMPLE_FRAME_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * | Header | Data payload | CRC |
 */

#pragma pack(1)
enum topics {
    TOPICS_MIN   = 0x00,
    TOPICS_SEN5x = 0x01,
    // TOPICS_MOISTURE,
    TOPIC_MAX,
};

typedef struct
{
    enum topics topic; /*msg type or DataId*/
    uint8_t dataLength;
    uint8_t *data;     /*actual data of payload*/
    uint16_t crc;
} Frame_t;

Frame_t *parsePayload(uint8_t *payload, uint8_t length);
void deleteFrame(Frame_t *frame);

uint16_t crc16_ccitt(const uint8_t *data, size_t length);

#endif
