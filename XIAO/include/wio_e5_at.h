#ifndef _AT_RADIO_H_
#define _AT_RADIO_H_
#include "Frame.h"
#include <SoftwareSerial.h>
#include <Stream.h>
#include <Wire.h>
#ifndef SERIAL_DEBUG
#define SERIAL_DEBUG Serial
#endif

#define SOFTWARE_SERIAL

#define boolToOn(_) (_ ? "ON" : "OFF")

#define REGION_EU868
/**
 * @attention MHz 868->868000000 Hz
 **/
#if defined(REGION_AS923)

#define RF_FREQUENCY 923 // Hz

#elif defined(REGION_AU915)

#define RF_FREQUENCY 915 // Hz

#elif defined(REGION_CN470)

#define RF_FREQUENCY 470 // Hz

#elif defined(REGION_CN779)

#define RF_FREQUENCY 779 // Hz

#elif defined(REGION_EU433)

#define RF_FREQUENCY 433 // Hz

#elif defined(REGION_EU868)

#define RF_FREQUENCY 868 // Hz

#elif defined(REGION_KR920)

#define RF_FREQUENCY 920 // Hz

#elif defined(REGION_IN865)

#define RF_FREQUENCY 865 // Hz

#elif defined(REGION_US915)

#define RF_FREQUENCY 915 // Hz

#elif defined(REGION_RU864)

#define RF_FREQUENCY 864 // Hz

#else
#error "Please define a frequency band in the compiler options."
#endif

/*!
 * \brief Represents the bandwidth values for LoRa packet type
 */
typedef enum {
    LORA_BW_500 = 6,
    LORA_BW_250 = 5,
    LORA_BW_125 = 4,
} RadioLoRaBandwidths_t;

const RadioLoRaBandwidths_t Bandwidths[] = {LORA_BW_125, LORA_BW_250, LORA_BW_500};

/*!
 * \brief Represents the possible spreading factor values in LoRa packet types
 */
typedef enum {
    LORA_SF5  = 0x05,
    LORA_SF6  = 0x06,
    LORA_SF7  = 0x07,
    LORA_SF8  = 0x08,
    LORA_SF9  = 0x09,
    LORA_SF10 = 0x0A,
    LORA_SF11 = 0x0B,
    LORA_SF12 = 0x0C,
} RadioLoRaSpreadingFactors_t;

/*!
 * \brief Represents the CRC mode for LoRa packet type
 */
typedef enum {
    LORA_CRC_OFF = 0x00, //!< CRC not used
    LORA_CRC_ON  = 0x01, //!< CRC activated
} RadioLoRaCrcModes_t;

/*!
 * \brief Represents the IQ mode for LoRa packet type
 */
typedef enum {
    LORA_IQ_NORMAL   = 0x00,
    LORA_IQ_INVERTED = 0x01,
} RadioLoRaIQModes_t;

/*!
 * \brief Represents the IQ mode for LoRa packet type
 */
typedef enum {
    LORA_PUBLIC_OFF = 0x00,
    LORA_PUBLIC_ON  = 0x01,
} RadioLoRaPublic_t;

class Radio : public Stream {
  public:
#ifdef SOFTWARE_SERIAL
    Radio(SoftwareSerial &serial, uint16_t frequency, RadioLoRaSpreadingFactors_t sf, RadioLoRaBandwidths_t bw, uint8_t tx_pr, uint8_t rx_pr, uint8_t power, RadioLoRaCrcModes_t crc, RadioLoRaIQModes_t iq, RadioLoRaPublic_t LoRawan);
#else
    Radio(HardwareSerial &serial, uint16_t frequency, RadioLoRaSpreadingFactors_t sf, RadioLoRaBandwidths_t bw, uint8_t tx_pr, uint8_t rx_pr, uint8_t power, RadioLoRaCrcModes_t crc, RadioLoRaIQModes_t iq, RadioLoRaPublic_t LoRawan);
#endif

    typedef enum {
        LOWPOWER,
        RX,
        RX_TIMEOUT,
        RX_ERROR,
        TX,
        TX_TIMEOUT,
    } LoRa_state;
    LoRa_state state = LOWPOWER;

    void begin();
    int ATCommand(const char *expected_response, int timeout, const char *command_format, const char *command_args);
    void sendPayload(String payload);
    void sendPayload(Frame_t frame);
    void sendPayload(uint8_t *payload, uint8_t size);

    void receivePacket(uint32_t time_out);

    bool isTopic(Frame_t &frame, enum topics topic);

    virtual int available() override {
        return _serial.available();
    }

    virtual int read() override {
        return _serial.read();
    }

    virtual int peek() override {
        return _serial.peek();
    }

    virtual size_t write(uint8_t data) override {
        return _serial.write(data);
    }

    // void stateMachine();
    void ChangeLowPower();

    String RadioGetSF();
    String RadioGetSF(RadioLoRaSpreadingFactors_t sf);
    int RadioGetBW();
    int RadioGetBW(RadioLoRaBandwidths_t bw);

  private:
#ifdef SOFTWARE_SERIAL
    SoftwareSerial &_serial;
#else
    HardwareSerial &_serial;
#endif
    bool _isNew = false;
    String _string;

    // uint8_t _srcMac[6];
    // uint8_t _dstMac[6];

    /*LoRa configuration param*/
    uint16_t _freq;
    RadioLoRaSpreadingFactors_t _sf;
    RadioLoRaBandwidths_t _bw;
    uint8_t _tx_pr;
    uint8_t _rx_pr;
    uint8_t _power;
    bool _crc;
    bool _iq;
    bool _loRawan;
};

#endif // __RADIO_H__
