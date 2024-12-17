# ESP32 based irrigation system

This is an ESP32 WROVER module based irrigation project. The list of features intended to be part of this are listed below

- Interface with a DRV8833 H-bridge driver for solenoid control
- Interface with a 1.54" Waveshare E-Ink display module for system status display
- Run a HTTP server over WiFi for configuration of the system


The application is built using the ESP-IDF sdk, v5.2.2. It is targeted towards being a low power system. As such most of the time, the ESP32 is in deep sleep mode. The chosen solenoid for this project is also a bi-stable solenoid that has a permanent magnet hold the plunger in place and only requiring a pulse to change states.

## Requirements
- ESP-IDF SDK 5.2.2
- vscode
- esp-idf extension for vscode (optional)

### Update Log
Made the http server class use a singleton design pattern. The http_server also inherits a Subject class with a registerObserver method. This method is used to intialize a member observer to call the port Supervisor objects updateConfig method. This allows updates to the port configuration when a payload is received via the http /config endpoint.


