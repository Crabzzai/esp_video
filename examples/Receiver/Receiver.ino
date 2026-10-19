#include <EspReceiver.h>

EspReceiver receiver;

const char* ssid = "ESP_VIDEO";
const char* password = "VerySecurePassword123"; // needs at least 8+ characters
const uint16_t udpPort = 1234;

void setup() {
    Serial.begin(115200);

    if (!receiver.begin(udpPort, ssid, password)) {
        Serial.println("Failed to initialize receiver.");
        while (true) {
            delay(1000); 
        }
    }
}

void loop() {
    receiver.loop();
}