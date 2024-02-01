#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <IRremote.h>
#include <ArduinoJson.h>
#include "soc/rtc_wdt.h"

// WiFi credentials
const char *ssid = "your_ssid";
const char *password = "your_password";

// Infrared sender object
IRsend irsend;

// Asynchronous Web Server on port 80
AsyncWebServer server(80);

void setup() {
  // Serial communication initialization
  Serial.begin(115200);

  // IR sender initialization
  irsend.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print connection details
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  // Define a route to handle receiving IR hex codes via POST request
  server.on("/sendIRHex", HTTP_POST, 
    // Lambda function to handle the POST request
    [](AsyncWebServerRequest *request) {},
    NULL, 
    // Lambda function to process the data received in chunks
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
      // JSON parsing
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject((const char*)data);

      // Check if JSON parsing is successful
      if (!root.success()) {
          request->send(400, "text/plain", "Invalid JSON");
          return;
      }

      // Extract IR code from JSON
      String codeValue = root["code"].asString();

      // Convert hexadecimal string to uint32_t
      uint32_t hex = strtoul(codeValue.c_str(), NULL, 16);
      Serial.print("Received by POST: 0x");
      Serial.println(hex, HEX);

      // Send IR code
      irsend.sendNEC(hex, 32);

      // Respond to the client
      request->send(200, "text/plain", "Code Sent");
  });

  // Start the server
  server.begin();
}

void loop() {
  // Minimal delay in the loop
  delay(1);
}
