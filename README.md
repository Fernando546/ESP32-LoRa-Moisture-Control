# ESP32-LoRa-Moisture-Control

This repository contains code for the ESP32-Soil-Moisture-Control, responsible for automating soil moisture monitoring and water management.

# Key Activities:
Receiving Instructions: The controller receives commands from the ESP32-LoRa-Bridge to adjust its operations.
Soil Humidity Measurement: It periodically measures soil moisture levels using a connected sensor.
Water Control: Based on the received instructions and sensor data, it controls an electrovalve to regulate water flow.
Data Transmission: The collected sensor data and system status are sent to a web server via the ESP32-LoRa-Bridge for monitoring and management.
This system ensures efficient irrigation control by responding to real-time soil conditions.
