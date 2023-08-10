#include "SensorPayload.h"

// Create arrays of strings corresponding to the enum values
#define X( name, str ) str,
const char *dataIDStrings[] = {
    dataID_ENUM_LIST };
#undef X

// Function to convert enum to string
const char *dataIDToString( enum dataID_t value ) {
    if ( value >= ID_MIN && value < ID_MAX ) {
        return dataIDStrings[value];
    }
    return "Unknown";
}
