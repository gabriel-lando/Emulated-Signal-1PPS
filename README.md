# ESP32 Signal 1PPS

ESP32 firmware that generates a **1 PPS (1 pulse-per-second)** signal using the high-resolution `esp_timer` API. Wi-Fi and Bluetooth are disabled at startup to minimize timing jitter.

## Signal

| Parameter   | Value                    |
| ----------- | ------------------------ |
| Period      | 1 s                      |
| Pulse width | 100 ms high / 900 ms low |
| Signal pin  | GPIO 0                   |
| LED pin     | GPIO 8 (built-in)        |

## Requirements

- ESP32 board (tested on ESP32-C3)
- [Arduino IDE](https://www.arduino.cc/en/software) with the [ESP32 Arduino core](https://github.com/espressif/arduino-esp32)

## Usage

1. Open `ESP32-Signal-1PPS.ino` in the Arduino IDE.
2. Select your ESP32 board and port.
3. Upload the sketch.

The LED and GPIO 0 will pulse once per second (100 ms high).
