#include "sensor_sen5x.h"
#include "main.h"

PayloadSEN5x::PayloadSEN5x(SensirionI2CSen5x handler)
    : SensorPayload<SensirionI2CSen5x>(handler), _sen5x(handler) {
    // Initialize specific data members for SEN5X here (if needed)
    _sen5x.begin(Wire);
}

uint16_t PayloadSEN5x::init() {
    // Implement the initialization logic for SEN5X here
    uint16_t error;
    char errorMessage[256];
    error = _sen5x.deviceReset();
    if (error) {
        SERIAL_DEBUG.print("Error trying to execute deviceReset(): ");
        errorToString(error, errorMessage, 256);
        SERIAL_DEBUG.println(errorMessage);
    }
    float tempOffset = 0.0;
    error            = _sen5x.setTemperatureOffsetSimple(tempOffset);
    if (error) {
        SERIAL_DEBUG.print("Error trying to execute setTemperatureOffsetSimple(): ");
        errorToString(error, errorMessage, 256);
        SERIAL_DEBUG.println(errorMessage);
    } else {
        SERIAL_DEBUG.print("Temperature Offset set to ");
        SERIAL_DEBUG.print(tempOffset);
        SERIAL_DEBUG.println(" deg. Celsius (SEN54/SEN55 only)");
    }

    // Start Measurement
    error = _sen5x.startMeasurement();
    if (error) {
        SERIAL_DEBUG.print("Error trying to execute startMeasurement(): ");
        errorToString(error, errorMessage, 256);
        SERIAL_DEBUG.println(errorMessage);
    }
    return 0;
}

String PayloadSEN5x::toPayloadString() {
// Add your code to convert data to payload string here
#ifdef DEVICE_SEN55
    _sen5x.readMeasuredValuesAsIntegers(massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0, massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex, noxIndex);
    _frame.dataLength = 16;
#else
    int16_t __noxIndex;
    _sen5x.readMeasuredValuesAsIntegers(massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0, massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex, __noxIndex);
    _frame.dataLength = 14;
#endif

    _frame.topic = TOPICS_SEN5x;

    // empty the data vector
    _frame.data.clear();

    // make the value into hex payload string
    _frame.data.push_back((uint8_t)(massConcentrationPm1p0 >> 8));
    _frame.data.push_back((uint8_t)(massConcentrationPm1p0 & 0xFF));
    _frame.data.push_back((uint8_t)(massConcentrationPm2p5 >> 8));
    _frame.data.push_back((uint8_t)(massConcentrationPm2p5 & 0xFF));
    _frame.data.push_back((uint8_t)(massConcentrationPm4p0 >> 8));
    _frame.data.push_back((uint8_t)(massConcentrationPm4p0 & 0xFF));
    _frame.data.push_back((uint8_t)(massConcentrationPm10p0 >> 8));
    _frame.data.push_back((uint8_t)(massConcentrationPm10p0 & 0xFF));
    _frame.data.push_back((uint8_t)(ambientHumidity >> 8));
    _frame.data.push_back((uint8_t)(ambientHumidity & 0xFF));
    _frame.data.push_back((uint8_t)(ambientTemperature >> 8));
    _frame.data.push_back((uint8_t)(ambientTemperature & 0xFF));
    _frame.data.push_back((uint8_t)(vocIndex >> 8));
    _frame.data.push_back((uint8_t)(vocIndex & 0xFF));
#ifdef DEVICE_SEN55
// _frame.data.push_back((uint8_t)(noxIndex >> 8));
// _frame.data.push_back((uint8_t)(noxIndex & 0xFF));
#endif
    char data[256];
    sprintf(data, "%d,%d,%d,%d,%d,%d,%d", massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0, massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex);
    SERIAL_DEBUG.println("String: " + String(data));

    for (int i = 0; i < _frame.dataLength; i++) {
        SERIAL_DEBUG.print(_frame.data[i], HEX);
        SERIAL_DEBUG.print(" ");
    }
    SERIAL_DEBUG.println();

    return packFrame(_frame);
}
