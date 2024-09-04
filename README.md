# WoL-Alternatives

## ESP32 Telegram Controlled Servo with Temperature Monitoring

This project enables you to control a servo motor via Telegram, monitor the temperature inside a PC case, and ping a specific host to check its status. The servo motor can be used to simulate pressing the power button on a PC. Thats literally Wake on LAN Alternatives (dumbb BUTT ITS WORK)

## Features

- Servo Control: Arm and trigger a servo motor via Telegram.
- Temperature Monitoring: Read and report the temperature inside a PC case.
- Host Ping: Check the availability of a specific host via ping.
- Singgle User Auth
## Screenshots

![image](https://github.com/user-attachments/assets/8868b87c-c727-4ed8-a201-fc4b90d261a9) ![image](https://github.com/user-attachments/assets/13f5cf77-c998-413a-943a-05d6d0ec6dd0)



## Commands

- **/start:** Start interaction with the bot.
- **/status:** Get the current system status.
- **/armed:** Arm the servo motor for 10 seconds.
- **/trigger:** Trigger the servo motor if armed and the host is not responding to ping(off).
- **/temp:** Get the current temperature inside the PC case
- **/force code :** Force trigger the servo motor with the correct security code. The code format is 121 followed by the current day and month (e.g., 121412 for 4th December).



## Environment Setup:

Hardware : 
- ESP32 development board
- Servo motor connected to pin 13
- DHT11 temperature sensor connected to pin 15

Libraries:
- WiFi.h
- WiFiClientSecure.h
- UniversalTelegramBot.h
- ESP32Servo.h
- DHT11.h
- [ESP32Ping.h](https://github.com/marian-craciunescu/ESP32Ping)
- time.h


