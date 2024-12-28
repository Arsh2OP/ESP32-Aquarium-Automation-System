# Smart Aquarium System 🐠🌿

A smart aquarium management system powered by ESP32. This project allows you to automate and monitor various aspects of your aquarium, including temperature, relay control for devices like pumps or lights, and time scheduling. It uses a DS18B20 temperature sensor, an RTC module, and a relay for automation.

---

## Features 🌟

- *Real-Time Temperature Monitoring:* Monitors aquarium water temperature and displays it on the web interface.
- *Time-Driven Control:* Uses an RTC to turn devices (e.g., pump, lights) on or off based on time.
- *Manual Override:* Allows manual control of devices via a web interface.
- *Temperature-Based LED Blinking:* If the water temperature drops below 25°C, the internal ESP32 LED blinks as an alert.
- *Wi-Fi Connectivity & Auto Reconnection:* Connects to Wi-Fi and auto reconnects if the connection is lost.
- *Web Interface:* View real-time data including current time, date, temperature, and relay state.

---

## Components 🧰

- *ESP32 Development Board*
- *DS18B20 Temperature Sensor*
- *DS3231 RTC Module*
- *Relay Module (for controlling devices)*
- *LED for temperature-based alerting*

---

## Wiring Diagram ⚡

1. *DS18B20 Temperature Sensor:*
   - Connect *VCC* to 3.3V
   - Connect *GND* to GND
   - Connect *Data Pin* to GPIO 4 (or any other suitable GPIO)

2. *DS3231 RTC Module:*
   - Connect *SDA* to GPIO 21
   - Connect *SCL* to GPIO 22

3. *Relay Module:*
   - Connect *VCC* to 3.3V
   - Connect *GND* to GND
   - Connect *Control Pin* to GPIO 23

---

## How to Use 🔧

1. *Setup:*
   - Open the Arduino IDE and install the necessary libraries (Wire, RTClib, OneWire, DallasTemperature, WiFi).
   - Configure your Wi-Fi credentials in the code:  
     cpp
     const char* ssid = "YourSSID";        // Replace with your Wi-Fi SSID
     const char* password = "YourPassword";  // Replace with your Wi-Fi password
     

2. *Upload the Code:*
   - Upload the code to your ESP32 board using Arduino IDE.

3. *Access the Web Interface:*
   - Once uploaded, open the Serial Monitor to find the IP address assigned to your ESP32.
   - Open a browser and enter the IP address to view the control panel.

4. *Control and Monitor:*
   - You can monitor the current time, date, and temperature.
   - Control the relay devices and enable manual override from the web interface.

---

## Code Overview 📑

1. *Wi-Fi and RTC Setup:*
   - The system connects to Wi-Fi and checks the connection every 10 seconds.
   - Uses the DS3231 RTC module to fetch current time and date.
   
2. *Temperature Monitoring:*
   - The DS18B20 temperature sensor is polled every second.
   - If the temperature drops below 25°C, the internal ESP32 LED blinks to alert the user.

3. *Relay Control:*
   - Devices connected to the relay are controlled based on time intervals (from the RTC) or manually via the web interface.

4. *Web Interface:*
   - Displays current time, date, and temperature, and allows control of the relay and manual override functionality.

---

## Web Interface 📱

The web interface provides the following functionalities:

- *Display Current Time & Date:* Updates every second.
- *Temperature Display:* Shows real-time temperature from the DS18B20 sensor.
- *Relay Control:* Buttons to turn on/off the relay and reset to automatic control based on RTC.
- *Manual Override:* Allows you to manually control the relay.

---

## Code Snippet 📜

Here is a snippet of the code for the web interface and temperature reading:

```cpp

---
client.print("<html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>ESP32 Relay Control</title><style>");
client.print("/* General Styles */ * { margin: 0; padding: 0; box-sizing: border-box; } body { font-family: 'Arial', sans-serif; background: linear-gradient(135deg, #6e7e88, #4d8b9d); color: #fff; text-align: center; padding: 20px; } h1 { font-size: 3em; margin-bottom: 20px; color: #fff; text-shadow: 2px 2px 5px rgba(0, 0, 0, 0.3); } .content { background-color: rgba(255, 255, 255, 0.2); border-radius: 15px; padding: 30px; margin-bottom: 20px; box-shadow: 0 6px 15px rgba(0, 0, 0, 0.3); } .data-info { font-size: 1.5em; margin: 20px 0; color: #f1f1f1; } .buttons { margin-top: 20px; } button { background-color: #4CAF50; color: white; font-size: 1.3em; padding: 12px 25px; border: none; border-radius: 10px; cursor: pointer; transition: all 0.3s ease; margin: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); } button:hover { background-color: #45a049; box-shadow: 0 6px 12px rgba(0, 0, 0, 0.3); } button:active { transform: scale(0.95); } .icon { font-size: 2em; margin-right: 10px; } @media screen and (max-width: 600px) { h1 { font-size: 2.2em; } .content { padding: 20px; } .data-info { font-size: 1.2em; } button { padding: 12px 20px; font-size: 1.1em; } }</style>");
client.print("<script>");
client.print("function updateTime() {");
client.print("  var currentTime = new Date();");
client.print("  document.getElementById('time').innerHTML = currentTime.getHours() + ':' + (currentTime.getMinutes() < 10 ? '0' : '') + currentTime.getMinutes() + ':' + (currentTime.getSeconds() < 10 ? '0' : '') + currentTime.getSeconds();");
client.print("  setTimeout(updateTime, 1000);");
client.print("}");
client.print("window.onload = updateTime;");
client.print("</script></head><body><h1>ESP32 Relay Control</h1><div class=\"content\"><p class=\"data-info\">Current Time: <span id=\"time\">" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + "</span></p>");
client.print("<p class=\"data-info\">Current Date: " + String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + "</p>");
client.print("<p class=\"data-info\">Relay State: " + String(relayState ? "ON" : "OFF") + "</p>");
client.print("<p class=\"data-info\">Manual Override: " + String(manualOverride ? "Enabled" : "Disabled") + "</p>");
client.print("<p class=\"data-info\" id=\"temperature\">Temperature: " + String(temperature) + "°C</p>");
client.print("<div class=\"buttons\"><a href=\"/relay/on\"><button><i class=\"icon\">🔌</i> Turn ON</button></a><a href=\"/relay/off\"><button><i class=\"icon\">❌</i> Turn OFF</button></a><a href=\"/relay/reset\"><button><i class=\"icon\">🔄</i> Reset to RTC Control</button></a></div></div></body></html>");

---
## License 📜

This project is licensed under the *MIT License*.

---

## Contact 📧

For any questions or suggestions, feel free to contact us:

- 📧 Email: shravindsingh007@gmail.com.com
---

*Developed with ❤ for Smart Aquarium Management.*

