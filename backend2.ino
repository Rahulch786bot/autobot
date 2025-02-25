#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "HTTPClient.h"

// WiFi credentials
const char* ssid = "vivo V29e";
const char* password = "rahulch786";

// API endpoint of the frontend (update this with your actual URL)
const char* FRONTEND_API_URL = "https://rahulch786bot.github.io/autobot/api/update";  

#define RELAY_NO true   // Set to true if relays are Normally Open
#define NUM_RELAYS 6
int relayGPIOs[NUM_RELAYS] = {2, 26, 27, 25, 33, 32};

AsyncWebServer server(80);

// Function to send relay status to frontend
void notifyFrontend(int relay, int state) {
    HTTPClient http;
    String url = String(FRONTEND_API_URL) + "?relay=" + String(relay) + "&state=" + String(state);

    http.begin(url);
    int httpResponseCode = http.GET();
    http.end();

    Serial.printf("Notified frontend: Relay %d -> %d (Response: %d)\n", relay, state, httpResponseCode);
}

// Handle relay control requests
void handleRelayControl(AsyncWebServerRequest *request) {
    if (request->hasParam("relay") && request->hasParam("state")) {
        int relay = request->getParam("relay")->value().toInt();
        int state = request->getParam("state")->value().toInt();

        if (relay >= 1 && relay <= NUM_RELAYS) {
            digitalWrite(relayGPIOs[relay - 1], RELAY_NO ? !state : state);
            Serial.printf("Relay %d set to %d\n", relay, state);
            notifyFrontend(relay, state);  // Notify the frontend
        }
    }
    request->send(200, "text/plain", "OK");
}

// Setup function
void setup() {
    Serial.begin(115200);

    // Initialize relay GPIOs
    for (int i = 0; i < NUM_RELAYS; i++) {
        pinMode(relayGPIOs[i], OUTPUT);
        digitalWrite(relayGPIOs[i], RELAY_NO ? HIGH : LOW);
    }

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi, IP: " + WiFi.localIP().toString());

    // API endpoint for relay control
    server.on("/update", HTTP_GET, handleRelayControl);

    // Start server
    server.begin();
}

// Main loop
void loop() {}
