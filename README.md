# Fan Cooling System - Bare-Metal AVR

A bare-metal C project for the ATmega328P. Implements custom 1-Wire and I2C drivers, 
PWM fan control and tachometer reading via hardware timers and interrupts. No external libraries.


## Overview

This project implements a temperature-based fan cooling system on an ATmega328P microcontroller.
A DS18B20 sensor continuously measures the ambient temperature and adjusts the fan speed accordingly
via PWM, across four operating modes: Silent, Normal, Performance and Max. The system monitors fan RPM
via tachometer input and detects faults such as fan stall, underspeed and sensor disconnection.
Errors are signaled through a 20x4 I2C LCD display, a passive buzzer and four status LEDs, each with
a dedicated alarm priority. All drivers are implemented from scratch via direct register manipulation,
using only the avr-gcc toolchain.

## Hardware

The system is built around an Arduino UNO R3 (ATmega328P) connected to a DS18B20 temperature sensor,
an Arctic P12 PRO PWM fan, a 20x4 I2C LCD display and a passive piezo buzzer. The circuit is powered
by a 12V supply with a DC-DC step-down module providing 5V to the MCU and peripherals. The fan is
driven directly at 12V. The circuit is currently assembled on a breadboard.

### Components

| **Component**             | **Quantity** | **Notes**                                      |
| :------------------------ | :----------- | :--------------------------------------------- |
| Arduino UNO R3            | 1            | ATmega328P @ 16MHz                             |
| Arctic P12 PRO            | 1            | 4-pin PWM fan, 12V                             |
| DS18B20                   | 1            | 1-Wire temperature sensor                      |
| NHD-0420H1Z               | 1            | 20x4 LCD with PCF8574 I2C module               |
| Murata Buzzer             | 1            | Passive piezo buzzer                           |
| LED                       | 4            | Red, Yellow, Blue, Green                       |
| DC-DC Step-Down Module    | 1            | 12V → 5V                                       |
| 12V Power Supply          | 1            |                                                |
| Capacitor 47µF 25V        | 1            | Electrolytic, power supply decoupling          |
| Capacitor 330µF 25V       | 1            | Electrolytic, fan power decoupling             |
| Capacitor 100nF           | 2            | Ceramic, decoupling                            |
| Resistor 4.7kΩ            | 1            | 1-Wire bus pull-up                             |
| Resistor 100Ω             | 1            | Buzzer current limiting                        |
| Resistor 220Ω             | 4            | LED current limiting                           |
| Breadboard 380 tie points | 2            |                                                |
| Jumper Wires              | 22           |                                                |

### Schematic
The KiCad project file and a PDF export are available in the `hardware/` folder.

![Schematic](media/schematic.png)

