/**
 * @file main.c
 * @brief advanced example entry point
 * @version 0.0.1
 * @date 2023-08-09
 * @note
 * @attention
 * @author @Love4yzp
 */

#include "bsp_board.h"
#include "esp_log.h"
#include "frame.h"
#include "radio.h"
#include "sen5x.h"

static const char *TAG = "app_main";

#define VERSION "v0.0.1"

#define SENSECAP "\n\
   _____                      _________    ____         \n\
  / ___/___  ____  ________  / ____/   |  / __ \\       \n\
  \\__ \\/ _ \\/ __ \\/ ___/ _ \\/ /   / /| | / /_/ /   \n\
 ___/ /  __/ / / (__  )  __/ /___/ ___ |/ ____/         \n\
/____/\\___/_/ /_/____/\\___/\\____/_/  |_/_/           \n\
--------------------------------------------------------\n\
 Version: %s %s %s\n\
--------------------------------------------------------\n\
"

#define RF_FREQUENCY               868000000 // Hz
#define LORA_BANDWIDTH             0         // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR      12        // [SF7..SF12]
#define LORA_CODINGRATE            1         // [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
#define LORA_PREAMBLE_LENGTH       15        // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT        5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON       false

static RadioEvents_t RadioEvents;

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
    int i = 0;
    ESP_LOGI(TAG, "rssi:%d dBm, snr:%d dB, len:%d, payload:", rssi, snr, size);
    for (i = 0; i < size; i++) {
        printf("0x%x ", payload[i]);
    }
    printf("\n");

    SEN5xData_t sen5x_data;

    Frame_t *frame = parsePayload(payload, size);
    if (frame == NULL) {
        ESP_LOGE(TAG, "parsePayload error");
        return;
    }
    ESP_LOGI(TAG, "frame->type: %s", dataIDToString(frame->topic));

    switch (frame->topic) {
        case TOPICS_SEN5x:
            phraseSEN5xData(frame->data, &sen5x_data);
            prinSEN5xData(&sen5x_data);
            break;

        default:
            break;
    }

    deleteFrame(frame);
}

void app_main(void) {
    ESP_LOGI("", SENSECAP, VERSION, __DATE__, __TIME__);

    ESP_ERROR_CHECK(bsp_board_init());

    ESP_LOGI(TAG, "APP MAIN START");

    RadioEvents.RxDone = OnRxDone;
    Radio.Init(&RadioEvents);

    Radio.SetChannel(RF_FREQUENCY);

    Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                      LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                      0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
    Radio.SetMaxPayloadLength(MODEM_LORA, 255);

    Radio.Rx(0);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
