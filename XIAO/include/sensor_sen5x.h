#ifndef PAYLOAD_SEN5X_H
#define PAYLOAD_SEN5X_H
#include "Frame.h"
#include "SensorPayload.h"
#include <SensirionI2CSen5x.h>

#define DEVICE_SEN54

#if defined(DEVICE_SEN54)
#elif defined(DEVICE_SEN55)
#else
#error "Please define a device in the compiler options."
#endif

class PayloadSEN5x : public SensorPayload<SensirionI2CSen5x> {
  public:
    PayloadSEN5x(SensirionI2CSen5x handler);
    uint16_t init() override;
    String toPayloadString() override;

  private:
    uint16_t massConcentrationPm1p0;
    uint16_t massConcentrationPm2p5;
    uint16_t massConcentrationPm4p0;
    uint16_t massConcentrationPm10p0;
    int16_t ambientHumidity;
    int16_t ambientTemperature;
    int16_t vocIndex;
#ifdef DEVICE_SEN55
// int16_t noxIndex; // Sensor SEN54 does not support NOx
#endif

    SensirionI2CSen5x _sen5x;
};

#endif // PAYLOAD_SEN5X_H
