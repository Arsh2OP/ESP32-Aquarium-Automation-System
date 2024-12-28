#include <Wire.h>
#include <RTClib.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- RTC and Relay Setup ---
RTC_DS3231 rtc;
const int relayPin = 23;  // Relay control pin

// --- Temperature Sensor Setup ---
#define ONE_WIRE_BUS 4  // DS18B20 connected to GPIO 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// --- Internal LED Pin ---
const int ledPin = 2;  // Internal LED pin for blinking on low temp

// --- Wi-Fi Credentials ---
const char* ssid = "Arsh5G";        // Wi-Fi SSID
const char* password = "@Rsh1234";  // Wi-Fi Password

WiFiServer server(80);

// --- Control Flags ---
bool manualOverride = false;  // Manual override flag
bool relayState = false;      // Relay state (ON/OFF)
bool lowTempAlert = false;    // Flag for low temperature alert

// --- Wi-Fi Management Variables ---
unsigned long previousMillis = 0;
const long wifiCheckInterval = 10000;  // Wi-Fi reconnection interval (10 sec)

// --- LED Blink Variables ---
unsigned long lastBlinkMillis = 0;
const long blinkInterval = 500;  // Blink every 500ms

void setup() {
  Serial.begin(115200);

  // --- Initialize RTC ---
  Wire.begin(21, 22);  // SDA, SCL
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // --- Initialize Relay ---
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);  // Relay OFF initially

  // --- Initialize Internal LED ---
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // LED OFF initially

  // --- Initialize Temperature Sensor ---
  sensors.begin();

  // --- Connect to Wi-Fi ---
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // --- Start Web Server ---
  server.begin();
  Serial.println("Server started");
}

void loop() {
  DateTime now = rtc.now();
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // --- Print Temperature and Time to Serial Monitor ---
  Serial.print("Time: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.print(" | Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // --- Low Temperature LED Blinking Logic ---
  unsigned long currentMillis = millis(); // Consolidated variable for timing

  if (temperature < 25.0) {
    if (currentMillis - lastBlinkMillis >= blinkInterval) {
      lastBlinkMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin));  // Toggle LED
    }
    lowTempAlert = true;
  } else {
    digitalWrite(ledPin, LOW);  // Ensure LED is OFF
    lowTempAlert = false;
  }

  // --- Automatic Relay Control (If Not Manually Overridden) ---
  if (!manualOverride) {
    if (now.hour() >= 13 && now.hour() < 22) {
      if (!relayState) {
        digitalWrite(relayPin, LOW);  // Relay ON
        relayState = true;
        Serial.println("RTC Control: Relay turned ON");
      }
    } else {
      if (relayState) {
        digitalWrite(relayPin, HIGH);  // Relay OFF
        relayState = false;
        Serial.println("RTC Control: Relay turned OFF");
      }
    }
  }

  // --- Wi-Fi Reconnection Logic ---
  if (currentMillis - previousMillis >= wifiCheckInterval) {
    previousMillis = currentMillis;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi disconnected. Attempting to reconnect...");
      WiFi.disconnect();
      WiFi.begin(ssid, password);
      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 5000) {
        delay(500);
        Serial.print(".");
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nReconnected to Wi-Fi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("\nFailed to reconnect to Wi-Fi.");
      }
    }
  }

  // --- Handle Web Client Requests ---
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // Handle Manual Override Requests
    if (request.indexOf("/relay/on") != -1) {
      manualOverride = true;
      relayState = true;
      digitalWrite(relayPin, LOW);  // Relay ON
    } else if (request.indexOf("/relay/off") != -1) {
      manualOverride = true;
      relayState = false;
      digitalWrite(relayPin, HIGH);  // Relay OFF
    } else if (request.indexOf("/relay/reset") != -1) {
      manualOverride = false;  // Switch to automatic mode
    }

    // Handle Temperature Update Request
    if (request.indexOf("/temp") != -1) {
      // Serve temperature update in response to /temp
      client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
      client.print(temperature);
      client.print("\n");  // Add newline for time info
      client.print(now.hour());
      client.print(":");
      client.print(now.minute());
      client.print(":");
      client.print(now.second());
      delay(1);
      client.stop();
    } else {
      // Serve main HTML page
      client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nCache-Control: no-store, must-revalidate\r\n\r\n");
      client.print("<html><head><title>ESP32 Control</title>");
      client.print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
      client.print("<style>");
      client.print("body { font-family: Arial, sans-serif; background-color: #f2f2f2; text-align: center; padding: 20px;}");
      client.print("h1 { color: #4CAF50; font-size: 24px; margin-bottom: 20px;}");
      client.print("p { font-size: 18px; color: #333;}");
      client.print(".buttons { margin: 20px; display: flex; flex-direction: column; align-items: center;}");
      client.print(".buttons a { text-decoration: none; padding: 10px 20px; margin: 10px; background-color: #4CAF50; color: white; border-radius: 5px; transition: background-color 0.3s ease; font-size: 18px;}");
      client.print(".buttons a:hover { background-color: #45a049;}");
      client.print(".temperature { font-size: 32px; font-weight: bold; color: #ff6347; margin: 20px;}");
      client.print(".status { font-size: 20px; color: #555;}");
      client.print(".status.on { color: green;}");
      client.print(".status.off { color: red;}");
      client.print(".status-mode { font-size: 20px; color: #333; margin-top: 20px;}");
      client.print(".status-mode strong { color: #ff6347;}");
      client.print("</style>");
      client.print("</head><body>");
      client.print("<h1>ESP32 Relay Control</h1>");
      client.print("<p>Time: <span id='time'>--:--:--</span></p>");
      client.print("<p>Date: " + String(now.year()) + "-" + String(now.month()) + "-" + String(now.day()) + "</p>");
      client.print("<p class='temperature'>Temperature: <span id='temp'>" + String(temperature) + "°C</span></p>");
      client.print("<p class='status " + String(relayState ? "on" : "off") + "'>Relay State: " + String(relayState ? "ON" : "OFF") + "</p>");
      client.print("<p class='status-mode'>Override Mode: <strong>" + String(manualOverride ? "Manual" : "Automatic") + "</strong></p>");
      client.print("<div class='buttons'>");
      client.print("<a href='/relay/on'>🔌 Turn ON</a>");
      client.print("<a href='/relay/off'>❌ Turn OFF</a>");
      client.print("<a href='/relay/reset'>🔄 Reset</a>");
      client.print("</div>");
      client.print("<script>");
      client.print("setInterval(function(){");
      client.print("fetch('/temp').then(response => response.text()).then(data => {");
      client.print("var tempData = data.split('\\n');");
      client.print("var tempElement = document.getElementById('temp');");
      client.print("var timeElement = document.getElementById('time');");
      client.print("tempElement.textContent = tempData[0] + '°C';");
      client.print("timeElement.textContent = tempData[1];");
      client.print("});");
      client.print("}, 1000);");
      client.print("</script>");
      client.print("</body></html>");
      delay(1);
      client.stop();
    }
  }

  delay(1000);  // Prevent rapid updates
}
