#ifndef SENSOR_PAYLOAD_H
#define SENSOR_PAYLOAD_H
#include "wio_e5_at.h"
#include <Arduino.h>

enum dataID_t {
    SEN5X = 0x01,
    // MOISTURE,
};

template <typename T>
class SensorPayload {
  public:
    SensorPayload(T t);
    virtual ~SensorPayload();
    virtual uint16_t init()          = 0;
    virtual String toPayloadString() = 0;

  protected:
    T handler;
    Frame_t _frame;
};

template <typename T>
SensorPayload<T>::SensorPayload(T t)
    : handler(t) {
    // Additional initialization if needed
}

template <typename T>
SensorPayload<T>::~SensorPayload() {
    // Destructor implementation if needed
}

#endif // SENSOR_PAYLOAD_H
