# Automatic Rain Detection Smart Window Using STM32

## Project Overview
This project is an automatic rain detection smart window system built using the STM32F103C8T6 Blue Pill microcontroller. It detects rain using a rain sensor and automatically closes the window using a servo motor to help protect the room from rain.

When the sensor detects dry conditions continuously for 10 seconds, the window opens automatically.

## Objectives
- Detect rain automatically using a rain sensor.
- Close the window when rain is detected.
- Reopen the window after 10 seconds of continuous dry conditions.
- Indicate the system status using two LEDs.

## Hardware Components
- STM32F103C8T6 Blue Pill
- Rain sensor module
- Servo motor
- Red LED
- White LED
- Resistors and jumper wires
- Foam-board house model

## Software and Tools
- STM32CubeIDE
- STM32CubeMX
- Embedded C
- STM32 HAL Library
- Git and GitHub

## Working Principle
1. The STM32 continuously reads the digital output of the rain sensor.
2. When rain is detected, the servo moves the window to the closed position.
3. The red LED turns ON to indicate rain.
4. When the sensor remains dry for 10 continuous seconds, the servo moves the window to the open position.
5. The white LED turns ON to indicate the open-window state.

## Microcontroller Pin Connections
Rain sensor digital output - PA0 

White LED - PB0 

Red LED - PB1 

Servo signal - TIM2 Channel 2 output pin 

**Note:** Confirm the servo signal pin from the actual CubeIDE/CubeMX configuration before connecting it.

## Future Improvements
- Add a manual override switch.
- Add Wi-Fi or mobile notifications.
- Improve rain detection reliability with sensor filtering.

## Author
Developed as an academic mini project.

## Project Images
pj1.png
pj2.png
