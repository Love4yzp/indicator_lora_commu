#include "sensor_sen5x.h"
#include "wio_e5_at.h"
#include <Arduino.h>
#include <SensirionI2CSen5x.h>
#include <Wire.h>
#include <queue>
#include <stack>
#include <vector>
SoftwareSerial serial_lora(D2, D3);
Radio radio(serial_lora, RF_FREQUENCY, LORA_SF12, LORA_BW_125, 15, 15, 14, LORA_CRC_ON, LORA_IQ_NORMAL, LORA_PUBLIC_OFF);

SensirionI2CSen5x sen5x;
PayloadSEN5x payloadSEN5x(sen5x);

void wait_serial(void) {
    while (!SERIAL_DEBUG) {
        // Led_Blink(500);
        ;
    }
}

void setup() {
    // put your setup code here, to run once:
    SERIAL_DEBUG.begin(9600);
    delay(2000);
    wait_serial();
    SERIAL_DEBUG.println("Starting...");

    radio.begin();

    Wire.begin();
    payloadSEN5x.init();

    SERIAL_DEBUG.println("APP begin");
}

void loop() {
    // put your main code here, to run repeatedly:
    static int count               = 0;
    static unsigned long task_time = 0;
    static String test_string;
    if (millis() - task_time > 10000) {
        task_time   = millis();
        test_string = payloadSEN5x.toPayloadString();
        radio.sendPayload(test_string);
        SERIAL_DEBUG.printf("Send data %d\r\n", count++);
        // if(radio.state == TX)
        // {
        //   radio.ChangeLowPower();
        // }
    }
}
