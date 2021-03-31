# RaspberryPi-Bare-Metal-Aarch64
A bare metal program written in Arm64 **assembly** and **C** language that runs on a Raspberry PI without any OS.

The goal of this project is just to demonstrate how to write a basic Operating System for Raspberry Pi that can interact with the hardware and display some output via UART serial and HDMI display.

## Features of this project

- **Memory Management**: Paging techniques, Memory allocation, etc.
- **Process Scheduling**: Preemptive Process Scheduling algorithm based on Priority
- **Basic IO**: Communication over UART, Display video output via HDMI
- **Interrupts & Exceptions**: Added interrupt handlers for supported ARM64 interrupts.
- **Graphics**: Ability to draw text on the HDMI output
