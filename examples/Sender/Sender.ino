#include <EspStreamer.h>

EspStreamer streamer;

const char* ssid = "ESP_VIDEO";
const char* password = "VerySecurePassword123"; // needs at least 8+ characters

void setup() {
    Serial.begin(115200);

    if (!streamer.begin(ssid, password)) {
        Serial.println("Failed to initialize streamer.");
        while (true) {
            delay(1000); 
        }
    }
}

void loop() {
    streamer.loop();
}