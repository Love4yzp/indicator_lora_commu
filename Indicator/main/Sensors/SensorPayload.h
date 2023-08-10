#ifndef SENSOR_PAYLOAD_H
#define SENSOR_PAYLOAD_H
#include "frame.h"

// Define the X-macro for enums and their string representations
// corresponding to the `topics` enum values
#define dataID_ENUM_LIST \
    X(ID_MIN, "ID_MIN")  \
    X(SEN5X, "SEN5X")    \
    X(ID_MAX, "ID_MAX")

// Define the enum
#define X(name, str) name,
enum dataID_t {
    dataID_ENUM_LIST
};
#undef X

// enum dataID_t {
//     ID_MIN = 0x00,
//     SEN5X = 0x01,
//     // MOISTURE,
//     ID_MAX
// };

const char *dataIDToString(enum dataID_t value);
#endif
