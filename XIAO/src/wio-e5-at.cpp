#include "wio_e5_at.h"

#define SERIAL_DEBUG Serial

int clean_serial(Stream &serial) {
    while (serial.available()) {
        delay(2);
        // serial.println("Clearing Serial Incoming Buffer.");
        serial.read();
    }
    if (serial.read() == -1) {
        // serial.println("Incoming Buffer is Clear.");
        return 0;
    }
    return -1;
}
/**
 * [FREQUENCY],[SF],[BANDWIDTH],[TX PR],[RX PR],[TX POWER],[CRC],[IQ],[NET]]
 * RFCFG supports set frequency, SF, band width, TX preamble, RX preamble and TX power settings.
    TX and RX shares all configuration except "preamble length"
 *  For LoRa communication, it is strongly recommended to set RX preamble length longer than TX's.
 * Bandwidth only supports 125KHz / 250KHz / 500KHz.
*/

#ifdef SOFTWARE_SERIAL
// Constructor implementation for SoftwareSerial
Radio::Radio(SoftwareSerial &serial, uint16_t frequency, RadioLoRaSpreadingFactors_t sf, RadioLoRaBandwidths_t bw, uint8_t tx_pr, uint8_t rx_pr, uint8_t power, RadioLoRaCrcModes_t crc, RadioLoRaIQModes_t iq, RadioLoRaPublic_t LoRawan)
    : _serial(serial), _freq(frequency), _sf(sf), _bw(bw), _tx_pr(tx_pr), _rx_pr(rx_pr), _power(power), _crc(crc), _iq(iq), _loRawan(LoRawan) {
    // Add any necessary initialization code for the constructor here
    // For example, you can initialize member variables here.
}
#else
// Constructor implementation for HardwareSerial
Radio::Radio(HardwareSerial &serial, uint16_t frequency, RadioLoRaSpreadingFactors_t sf, RadioLoRaBandwidths_t bw, uint8_t tx_pr, uint8_t rx_pr, uint8_t power, RadioLoRaCrcModes_t crc, RadioLoRaIQModes_t iq, RadioLoRaPublic_t LoRawan)
    : _serial(serial), _freq(frequency), _sf(sf), _bw(bw), _tx_pr(tx_pr), _rx_pr(rx_pr), _power(power), _crc(crc), _iq(iq), _loRawan(LoRawan) {
    // Add any necessary initialization code for the constructor here
    // For example, you can initialize member variables here.
}
#endif

void Radio::begin() {
    _serial.begin(9600);

    String config_string = "";
    config_string += String(_freq) + ",";
    config_string += RadioGetSF(_sf) + ",";
    config_string += String(RadioGetBW(_bw)) + ",";
    config_string += String(_tx_pr) + ",";
    config_string += String(_rx_pr) + ",";
    config_string += String(_power) + ",";
    config_string += String(boolToOn(_crc)) + ",";
    config_string += String(boolToOn(_iq)) + ",";
    config_string += String(boolToOn(_loRawan));

    ATCommand("+MODE:", 1000, "AT+MODE=%s\r\n", "TEST");
    state = LOWPOWER;
    // ATCommand("+TEST: RFCFG", 1000, "AT+TEST=RFCFG,%s\r\n", "868,SF12,125,15,15,14,ON,OFF,OFF");
    ATCommand("+TEST: RFCFG", 2000, "AT+TEST=RFCFG,%s\r\n", config_string.c_str());
    if (state != TX) {
        SERIAL_DEBUG.println("failed init LoRa module, plese reset device and try again");
        for (;;)
            ;
    }
    SERIAL_DEBUG.println("LoRa module init success");
    state = LOWPOWER;
}

bool Radio::isTopic(Frame_t &frame, enum topics topic) {
    return frame.topic == topic;
}

int Radio::ATCommand(const char *expected_response, int timeout, const char *command_format, const char *command_args) {
    char send_buffer[256];
    char recv_buffer[256];
    if (state != LOWPOWER) {
        SERIAL_DEBUG.println("Radio is busy, please try again later");
        return -1;
    }
    snprintf(send_buffer, sizeof(send_buffer), command_format, command_args);

    SERIAL_DEBUG.print("<<<" + String(send_buffer));
    _serial.print(send_buffer); // Send the command to the Wio-E5

    int index = 0;
    if (expected_response == NULL) {
        state = TX;
        return 0;
    }

    SERIAL_DEBUG.print(">>>");
    uint32_t startMillis = millis();
    do {
        while (_serial.available() > 0) {
            char ch = _serial.read();
            if (index >= sizeof(recv_buffer) - 1) {
                SERIAL_DEBUG.println("recv_buff overflow");
                break;
            }
            recv_buffer[index++] = ch;
            SERIAL_DEBUG.print(ch);
            delay(3);
            if (strstr(recv_buffer, expected_response) != NULL) {
                SERIAL_DEBUG.println("\n");
                if (clean_serial(_serial) != ESP_OK)
                    continue;
                state = TX;
                return 0;
            }
        }
    } while (millis() - startMillis < timeout);
    state = TX_TIMEOUT;
    return -1;
}

void Radio::sendPayload(Frame_t frame) {
    sendPayload(packFrame(frame));
}

void Radio::sendPayload(uint8_t *payload, uint8_t size) {
    String payload_str = "";
    for (int i = 0; i < size; i++) {
        payload_str += String(payload[i], HEX);
    }
    sendPayload(payload_str);
}

void Radio::sendPayload(String payload) {
    ATCommand("+TEST: TXLRPKT", 1000, "AT+TEST=TXLRPKT,\"%s\"\r\n", payload.c_str());
    if (state != TX) {
        SERIAL_DEBUG.println("Send payload failed, please reset device and try again");
        delay(30);
        return;
    }
    state = LOWPOWER;
    SERIAL_DEBUG.println("Send payload successfully");
}

void Radio::receivePacket(uint32_t time_out) {
    const char *param_ack = "+TEST: LEN:";
    const char *data_ack  = "+TEST: RX \"";
    char recv_buffer[128];
    char *param_position;
    char *position;

    if (state != LOWPOWER) {
        SERIAL_DEBUG.println("Radio is busy, please try again later");
        return;
    }

    ATCommand("+TEST: RXLRPKT", 1000, "AT+TEST=RXLRPKT\r\n", ""); // switch to RX mode
    if (state != TX)                                              // didn't tramsmit suscessfully
    {
        SERIAL_DEBUG.println("Switch to RX mode failed, please reset device and try again");
        delay(30);
        return;
    }

    uint32_t startMillis = millis();
    do {
        if (_serial.available() > 0) {
            size_t len       = _serial.readBytesUntil('\n', recv_buffer, sizeof(recv_buffer) - 1);
            recv_buffer[len] = '\0'; // null terminate the string
            // check if it is a param ack
            param_position = strstr(recv_buffer, param_ack);
            if (param_position != NULL) {
                int len, rssi, snr;
                sscanf(param_position, "+TEST: LEN:%d, RSSI:%d, SNR:%d", &len, &rssi, &snr);
                SERIAL_DEBUG.printf("LEN:%d RSSI:%d, SNR:%d\r\n", len, rssi, snr);
                continue;
            }
            position = strstr(recv_buffer, data_ack);
            if (position != NULL) {
                char hexString[50] = {0};
                int ret            = sscanf(position, "+TEST: RX \"%[^\"]\"", hexString); // extract the payload
                clean_serial(_serial);
                if (ret == 1) {
                    _string = String(hexString);
                    SERIAL_DEBUG.println("Payload:" + _string);
                    _isNew = true;
                    state  = RX;
                    return;
                }
                state = RX_ERROR;
                return;
            }
        }
    } while (millis() - startMillis < time_out);
    state = RX_TIMEOUT;
    return;
}

String Radio::RadioGetSF() {
    return RadioGetSF(_sf);
}

void Radio::ChangeLowPower() {
    this->state = LOWPOWER;
}

String Radio::RadioGetSF(RadioLoRaSpreadingFactors_t sf) {
    switch (sf) {
        {
            case LORA_SF5:
                return "SF5";
                break;
            case LORA_SF6:
                return "SF6";
                break;
            case LORA_SF7:
                return "SF7";
                break;
            case LORA_SF8:
                return "SF8";
                break;
            case LORA_SF9:
                return "SF9";
                break;
            case LORA_SF10:
                return "SF10";
                break;
            case LORA_SF11:
                return "SF11";
                break;
            case LORA_SF12:
                return "SF12";
                break;
            default:
                break;
        }
    }
    return "";
}

int Radio::RadioGetBW() {
    return RadioGetBW(_bw);
}

int Radio::RadioGetBW(RadioLoRaBandwidths_t bw) {
    switch (bw) {
        case LORA_BW_500:
            return 500;
            break;
        case LORA_BW_250:
            return 250;
            break;
        case LORA_BW_125:
            return 125;
            break;
        default:
            break;
    }
    return -1;
}
