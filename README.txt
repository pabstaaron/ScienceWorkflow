-- UofU CHPC Science Workflow Project --

This repository contains all source files for the robotic portion of the UofU CHPC's Science
Workflow project.

The main application consists of a C program, ControlServer.c, that can be found under
ScienceWorkflow_Control_System. This program was designed to be ran on a Raspberry Pi 3 board
with I2C enabled.

When ran, ControlServer starts listening for commands over UDP on port 8080. A listing and
explanation of each command can be found in ScienceWorkflow_Control_Software/ControlAPI.txt.

When the application receives a valid command, it calculates an appropriate response and
either adjusts the device's motors and servos to match, or sends sensor data back to the
client machine, depending on the type of command received.

ControlServer.c interfaces with a Python library via the Python-C API in order to communicate
with a PCA9685 PWM chip. This library must be installed independantly. Instructions for
installing the library can be found at: https://learn.adafruit.com/adafruit-16-channel-servo-driver-with-raspberry-pi/using-the-adafruit-library

