
# Simple LoRa communication demo between SenseCAP Indicator and XIAO (Via Wio-E5)

## Overview
This demonstration showcases how to establish a basic LoRa communication between the SenseCAP Indicator and the XIAO board using the Wio-E5 as an intermediary. The SenseCAP Indicator retrieves sensor data from the XIAO, which is then transmitted via Wio-E5. The transmitted payload is subsequently received by the SenseCAP Indicator, which deciphers and outputs. the result and displays the data on its screen.

## Installation
The Indicator folder pertains to the SenseCAP Indicator, while the XIAO folder corresponds to the XIAO board.

For convenience, I utilize PlatformIO (integrated into VSCode) to manage the XIAO Arduino project. Alternatively, you can manage the project using the Arduino IDE. However, you will need to manually install the following libraries:

- plerup/EspSoftwareSerial@^8.1.0
- sensirion/Sensirion I2C SEN5X@^0.3.0
- sensirion/Sensirion Core@^0.6.0

If you're new to PlatformIO, you can refer to the [PlatformIO Getting Started Guide](https://docs.platformio.org/en/latest/ide/vscode.html#quick-start) for installing PlatformIO on VSCode. You may also find the following post on hackster.io helpful: [Mastering Project Compilation with PlatformIO](https://www.hackster.io/love4yzp/mastering-project-compilation-with-platformio-a0ff85).

### Components
- Devices:
  - [SenseCAP Indicator](https://www.seeedstudio.com/SenseCAP-Indicator-p-4739.html)
  - [XIAO](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html)
  - [Wio-E5](https://www.seeedstudio.com/Grove-LoRa-E5-STM32WLE5JC-p-4867.html)
- Sensors:
  - SEN54
  - ... (you can also use other sensors, but you need to modify the code)

## Usage
### Setting Up the XIAO
1. Connect the XIAO board to your Wio-E5 and computer using a USB cable.
2. Open the XIAO folder in VSCode.
3. Click the PlatformIO: Build button located at the lower left corner of VSCode.
4. Click the PlatformIO: Upload button located at the lower left corner of VSCode.
5. Open the Serial Monitor in VSCode and configure the baud rate to 9600 (as specified in the code).
6. The Serial Monitor will display the following message:

```
String: 76,80,81,81,5389,5990,980
0 4C 0 50 0 51 0 51 15 D 17 66 3 D4
CRC: 629
<<<AT+TEST=TXLRPKT,"010E004C005000510051150D176603D40629"
>>>+TEST: TX DONE
+TEST: TXLRPKT

Send payload successfully
Send data 1
```

### SenseCAP Indicator
#### Compilation and Flashing
1. Connect the SenseCAP Indicator to your computer via USB cable.
2. Activate IDF environment and navigate to the `Indicator` folder.
3. Run `idf.py -p PORT build flash monitor` to build and flash the project, and then observe the results in the serial output.
#### Monitor
```
I (95490) app_main: rssi:-22 dBm, snr:5 dB, len:18, payload:
0x1 0xe 0x0 0x4c 0x0 0x50 0x0 0x51 0x0 0x51 0x15 0xd 0x17 0x66 0x3 0xd4 0x6 0x29
W (95541) parsePayload: topic: 1
W (95541) parsePayload: dataLength: 14
W (95545) parsePayload: payload[0]: 00
W (95549) parsePayload: payload[1]: 4C
W (95554) parsePayload: payload[2]: 00
W (95558) parsePayload: payload[3]: 50
W (95563) parsePayload: payload[4]: 00
W (95567) parsePayload: payload[5]: 51
W (95572) parsePayload: payload[6]: 00
W (95576) parsePayload: payload[7]: 51
W (95580) parsePayload: payload[8]: 15
W (95585) parsePayload: payload[9]: 0D
W (95589) parsePayload: payload[10]: 17
W (95594) parsePayload: payload[11]: 66
W (95598) parsePayload: payload[12]: 03
W (95603) parsePayload: payload[13]: D4
I (95607) app_main: frame->type: SEN5X
I (95612) sen5x_: massConcentrationPm1p0: 76
I (95617) sen5x_: massConcentrationPm2p5: 80
I (95622) sen5x_: massConcentrationPm4p0: 81
I (95627) sen5x_: massConcentrationPm10p0: 81
I (95632) sen5x_: ambientHumidity: 5389
I (95636) sen5x_: ambientTemperature: 5990
I (95641) sen5x_: vocIndex: 980
```

## LVGL
In this project, we use SquareLine Studio to implement LVGL code to display the data on the screen.
the SquareLine Studio version in this project is 1.3.1.
