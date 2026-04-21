# Smart-Attendance-System
By scanning the RFID it marked present or unauthrized

Smart Classroom IoT System

This project is an IoT-based Smart Classroom System designed to automate student attendance and monitor classroom environmental conditions using modern IoT technologies. The system uses an RFID reader (RC522) to scan student ID cards and automatically record attendance through a NodeMCU ESP8266 microcontroller.

When a student scans their RFID card, the system verifies the card UID with the authorized list. If the card is valid, attendance is recorded and stored in Google Sheets through cloud connectivity. The system also provides real-time updates and notifications through the Blynk IoT dashboard, allowing remote monitoring using a mobile device.

Additionally, a DHT11 sensor is used to monitor classroom temperature and humidity. These environmental parameters are displayed on the Blynk dashboard for real-time monitoring. A buzzer alert system is included to provide feedback: one beep indicates an authorized card and two beeps indicate an unauthorized card.

The system also generates alerts when temperature or humidity exceeds predefined limits, improving classroom monitoring and safety.

Features
RFID-based automatic attendance system
Real-time IoT monitoring using NodeMCU ESP8266
Cloud data storage using Google Sheets
Mobile dashboard using Blynk IoT platform
Temperature and humidity monitoring using DHT11 sensor
Buzzer alerts for authorized and unauthorized access
Notifications and alerts for environmental conditions
Technologies Used
NodeMCU ESP8266
RFID RC522 Module
DHT11 Temperature & Humidity Sensor
Blynk IoT Platform
Google Sheets API
Arduino IDE

<img width="1536" height="1024" alt="ChatGPT Image Apr 21, 2026, 09_24_22 AM" src="https://github.com/user-attachments/assets/75bd4bf8-f431-4e09-9278-ea007c422bd1" />

<img width="960" height="1082" alt="WhatsApp Image 2026-04-21 at 9 15 47 AM" src="https://github.com/user-attachments/assets/ef787bd3-06d3-47b5-b3ad-a11e60931976" />

<img width="720" height="1197" alt="WhatsApp Image 2026-04-21 at 8 01 51 AM" src="https://github.com/user-attachments/assets/499ca1fc-c589-447f-9fef-d2929b6e858f" />
