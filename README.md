# IoT Air Quality Monitoring System with GPS Tracking

## Project Description  
This project implements an IoT-based air quality monitoring system using ESP32 microcontroller. It collects real-time environmental data including temperature, humidity, CO, CO₂, NH3, and PM2.5 levels while simultaneously tracking device location through GPS. Sensor data is transmitted to a remote server via GSM/GPRS for centralized monitoring. The system uses multiple MQ-series gas sensors combined with environmental compensation algorithms to provide actionable air quality insights.

## Key Features
- 6-in-1 environmental monitoring (Temp/Humidity/CO/CO₂/NH3/PM2.5)
- Real-time GPS geolocation tracking
- GSM-based cellular data transmission
- Calibration-adjusted sensor readings
- Serial monitoring interface
- LED status indication for particle detection

## Hardware Configuration
### Sensor Pin Mapping
| Component       | ESP32 Pin | Signal Type  |
|-----------------|-----------|--------------|
| DHT11           | GPIO27    | Digital      |
| MQ-7 (CO)       | GPIO39    | Analog       |
| MQ-135 (NH3)    | GPIO32    | Analog       |
| MQ-135 (CO₂)    | GPIO34    | Analog       |
| Dust Sensor      | GPIO36    | Analog       |
| GPS TX          | GPIO22    | Serial RX    |
| GPS RX          | GPIO23    | Serial TX    |
| GSM TX          | GPIO18    | Serial RX    |
| GSM RX          | GPIO19    | Serial TX    |
| Status LED      | GPIO13    | Digital Out  |

**Note:** Schematic diagrams (if present) may show simplified connections - verify actual pin assignments against this table.

## Key Components
- ESP32-WROOM Microcontroller
- SIM800L GSM/GPRS Module
- NEO-6M GPS Module
- D
