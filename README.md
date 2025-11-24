
# 🧊 Smart Refrigerator Smell Monitoring System

A simple IoT system that detects unusual odors inside a refrigerator using **MQ-135**, **MQ-4**, **DHT22**, and an **ESP32**.
The system sends real-time data to a web dashboard, and an admin panel allows adjusting thresholds manually (for demonstration).
A **buzzer** and **OLED display** provide local alerts.

## 📌 Features

* Detects refrigerator gas levels (MQ-135 & MQ-4)
* Monitors temperature & humidity (DHT22)
* OLED screen shows real-time readings
* Buzzer alarm for critical smell levels
* Web dashboard for live data
* Admin panel to change smell thresholds manually 
* Wi-Fi communication between ESP32 → Web App

## 🛒 Hardware Components

| Component                | Use                                  |
| ------------------------ | ------------------------------------ |
| ESP32 Dev Board (38 pin) | Main controller + WiFi               |
| MQ-135                   | Detects ammonia, CO₂, alcohol vapors |
| MQ-4                     | Detects methane & spoilage gases     |
| DHT22                    | Temperature & humidity               |
| OLED Display (I2C)       | Local display                        |
| Passive Buzzer           | Alert system                         |
| Jumper Wires             | Connections                          |
| Breadboard               | Testing                              |
| 5V Power                 | USB/Adapter                          |


## 🔌 Pin Connections

| Module    | ESP32 Pin  |
| --------- | ---------- |
| MQ-135 A0 | GPIO 34    |
| MQ-4 A0   | GPIO 35    |
| DHT22     | GPIO 27    |
| OLED SDA  | GPIO 21    |
| OLED SCL  | GPIO 22    |
| Buzzer    | GPIO 23    |
| Power     | 3.3V / GND |


## 🌐 Web App (Two Parts)

### **User Dashboard**

* Displays gas levels, temperature, humidity
* Shows system status: **Normal / Smell Detected**

### **Admin Panel**

* Manually adjust thresholds
* Used for demo: simulate spoiled food by increasing sensitivity

