#ifndef PAYLOAD_SEN5X_H
#define PAYLOAD_SEN5X_H
#include "SensorPayload.h"

#define DEVICE_SEN54

#if defined( DEVICE_SEN54 )
#elif defined( DEVICE_SEN55 )
#else
#error "Please define a device in the compiler options."
#endif

typedef union {
    struct
    {
        uint16_t massConcentrationPm1p0;
        uint16_t massConcentrationPm2p5;
        uint16_t massConcentrationPm4p0;
        uint16_t massConcentrationPm10p0;
        int16_t ambientHumidity;
        int16_t ambientTemperature;
        int16_t vocIndex;
#ifdef DEVICE_SEN55
        int16_t noxIndex;
#endif
    };

#ifdef DEVICE_SEN55
    int16_t data[8];
#else
    int16_t data[7];
#endif
} SEN5xData_t;

void phraseSEN5xData( uint8_t *data_arry, SEN5xData_t *SEN5x );
void prinSEN5xData( const SEN5xData_t *SEN5x );

#endif // PAYLOAD_SEN5X_H
