#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Secret world";       // Your WiFi SSID
const char* password = "123456789";      // Your WiFi Password

WebServer server(80);  // Start HTTP server on port 80

// Relay GPIO pins
const int relayPins[] = {27, 33, 13, 23, 21, 5};

// Relay to Lights Mapping (7 relays control 13 lights)
const int relayMapping[13] = {
  0, 1, 2, 3, 4, 5, 6,  // Lights 1-7 have individual relays
  0, 1, 2, 3, 4, 5      // Lights 8-13 share relays with 1-6
};

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize relays (set as OUTPUT and turn them OFF)
  for (int i = 0; i < 7; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);  // HIGH means relay OFF
  }

  // Define server routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();
  Serial.println("HTTP Server Started!");
}

void handleRoot() {
  server.send(200, "text/plain", "ESP32 Web Server Running");
}

void handleToggle() {
  if (server.hasArg("light")) {
    int light = server.arg("light").toInt();
    if (light >= 0 && light < 13) {
      int relay = relayMapping[light];
      digitalWrite(relayPins[relay], !digitalRead(relayPins[relay]));  // Toggle relay
      Serial.print("Toggled Light: ");
      Serial.println(light);
      server.send(200, "text/plain", "OK");
      return;
    }
  }
  server.send(400, "text/plain", "Invalid Request");
}

void loop() {
  server.handleClient();
}
