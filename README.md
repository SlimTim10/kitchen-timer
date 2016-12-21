# Kitchen Timer

## Description

A simple kitchen timer with two buttons and a buzzer for an alarm. On power up, the previously set alarm time will display (saved in EEPROM). Pressing button 2 repeatedly will set the timer (in increments of 1 minute, always starting back at 0 with the first press). Pressing button 1 will start the timer. When the time is up, the brief alarm will sound.

## Hardware

See file bom.org

## ISP Programming

The ATmega328P is loaded with the Arduino bootloader and can be programmed directly with an Arduino Uno.

1. Make the following connections to the ISP header:

| Arduino Uno | ISP Header |
| :--- | :--- |
| GND | 1 |
| 5V | 2 |
| 10 | 3 |
| 11 | 4 |
| 12 | 5 |
| 13 | 6 |

2. Load the sketch in the Arduino IDE.
3. Set `Tools -> Programmer -> Arduino as ISP`.
4. Select `Sketch -> Upload Using Programmer`.