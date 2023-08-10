#include "sen5x.h"
#include "esp_log.h"

void phraseSEN5xData(uint8_t *data_arry, SEN5xData_t *SEN5x) {
    for (uint8_t i = 0; i < sizeof(SEN5xData_t); i++) {
        SEN5x->data[i] = data_arry[2 * i] << 8 | data_arry[2 * i + 1];
    }
    // SEN5x->massConcentrationPm1p0 = (data_arry[0] << 8) | data_arry[1];
    // SEN5x->massConcentrationPm2p5 = (data_arry[2] << 8) | data_arry[3];
    // SEN5x->massConcentrationPm4p0 = (data_arry[4] << 8) | data_arry[5];
    // SEN5x->massConcentrationPm10p0 = (data_arry[6] << 8) | data_arry[7];
    // SEN5x->ambientHumidity = (data_arry[8] << 8) | data_arry[9];
    // SEN5x->ambientTemperature = (data_arry[10] << 8) | data_arry[11];
    // SEN5x->vocIndex = (data_arry[12] << 8) | data_arry[13];
    // #ifdef DEVICE_SEN55
    // SEN5x->noxIndex = (data_arry[14] << 8) | data_arry[15];
    // #endif
}

void prinSEN5xData(const SEN5xData_t *SEN5x) {
    static const char *TAG = "sen5x_";
    ESP_LOGI(TAG, "massConcentrationPm1p0: %d", SEN5x->massConcentrationPm1p0);
    ESP_LOGI(TAG, "massConcentrationPm2p5: %d", SEN5x->massConcentrationPm2p5);
    ESP_LOGI(TAG, "massConcentrationPm4p0: %d", SEN5x->massConcentrationPm4p0);
    ESP_LOGI(TAG, "massConcentrationPm10p0: %d", SEN5x->massConcentrationPm10p0);
    ESP_LOGI(TAG, "ambientHumidity: %d", SEN5x->ambientHumidity);
    ESP_LOGI(TAG, "ambientTemperature: %d", SEN5x->ambientTemperature);
    ESP_LOGI(TAG, "vocIndex: %d", SEN5x->vocIndex);
#ifdef DEVICE_SEN55
    ESP_LOGI(TAG, "noxIndex: %d", SEN5x->noxIndex);
#endif
}
