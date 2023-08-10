#include "Frame.h"

#define SERIAL_DEBUG Serial
// #define DEBUG
String packFrame(Frame_t frame) {
    uint8_t *packedData = (uint8_t *)malloc(2 + frame.dataLength + 2);
    if (packedData == NULL) {
        return String(); // Return an empty string if memory allocation fails
    }
    // Pack the frame topic
    packedData[0] = frame.topic;
    packedData[1] = frame.dataLength;

    // Pack the data payload
    for (size_t i = 0; i < frame.dataLength; i++) {
        packedData[2 + i] = frame.data[i];
    }
#ifdef DEBUG
    SERIAL_DEBUG.println("topic: " + String(frame.topic));
    SERIAL_DEBUG.println("dataLength: " + String(frame.dataLength));
    SERIAL_DEBUG.print("Data: ");
    for (size_t i = 2; i < 2 + frame.dataLength; i++) {
        SERIAL_DEBUG.print(packedData[i], HEX);
    }
    SERIAL_DEBUG.println();
#endif
    // Calculate CRC (for simplicity, we assume CRC is just the sum of all bytes)
    frame.crc = crc16_ccitt(packedData, 2 + frame.dataLength);

    // Pack the CRC
    packedData[2 + frame.dataLength]     = (frame.crc & 0xFF00) >> 8;
    packedData[2 + frame.dataLength + 1] = frame.crc & 0x00FF;

    SERIAL_DEBUG.println("CRC: " + String(frame.crc, HEX));
    // String packedFrame;
    char packedFrameBuffer[(2 + frame.dataLength + 2) * 2];
    for (size_t i = 0; i < 2 + frame.dataLength + 2; i++) {
        snprintf(&packedFrameBuffer[i * 2], 3, "%02X", packedData[i]); // 3 to include null terminator
    }

#ifdef DEBUG
    SERIAL_DEBUG.print("Packed frame: ");
    SERIAL_DEBUG.println(packedFrameBuffer);
#endif

    free(packedData);

    return String(packedFrameBuffer);
}

void deleteFrame(Frame_t *frame) {
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
