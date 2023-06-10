# EmbeddedIoT
This is not the source code for the commercial version.

The device's application program reads the rainfall sensor data and sends it over the 3G/4G telecommunications network, combining alerts via SMS, chat app, and LoRa.

Modules:
- STM32L1 MCUs
- GSM module
- LoRa module RFM95
- SD card module
- ...

Working principle:
- Rainfall data measured by the sensor is sent to the device. This data is stored and uploaded to the management server platform periodically.
- Can configure and monitor the device remotely via SMS.
- The device operates in energy-saving mode to ensure stability under extreme conditions.