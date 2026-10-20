#ifndef ESP_STREAMER_H
#define ESP_STREAMER_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_camera.h"
#include <EspSharedConfig.h>
#include <EspCameraSource.h>

class EspStreamer {
    private:
        WiFiUDP _udp;
        char* _target_ip;
        uint16_t _udp_port;
        static const uint16_t MAX_PAYLOAD = 1436; // safely below 1472 MTU limit
        const char* _ssid;
        const char* _password;
        EspCameraSource cameraSource;

        inline void send(camera_fb_t* frame) {
            if (!frame) return;

            size_t bytes_sent = 0;
            uint16_t chunk_id = 0;

            while (bytes_sent < frame->len) {
                size_t chunk_size = (frame->len - bytes_sent > MAX_PAYLOAD) ? MAX_PAYLOAD : (frame->len - bytes_sent);

                _udp.beginPacket(_target_ip, _udp_port);
                _udp.write((uint8_t*)&chunk_id, 2);
                _udp.write((uint8_t*)&frame->len, 4);
                _udp.write(frame->buf + bytes_sent, chunk_size);
                _udp.endPacket();

                bytes_sent += chunk_size;
                chunk_id++;
            }
        }

    public:
        inline bool begin(const char* ssid, const char* password) {
            _ssid = ssid;
            _password = password;
            WiFi.mode(WIFI_STA);
            WiFi.begin(_ssid, _password);
            Serial.print("Connecting to WiFi");
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
            }
            Serial.println();
            Serial.print("Connected to WiFi. IP address: ");
            Serial.println(WiFi.localIP());
            Serial.println();

            while (true) {
                int packetSize = _udp.parsePacket();
                if (packetSize > sizeof(Command)) {
                    Command cmd;
                    _udp.read((uint8_t*)&cmd, sizeof(cmd));

                    IPAddress receiver_ip = _udp.remoteIP();

                    _target_ip = strdup(receiver_ip.toString().c_str());
                    _udp_port = cmd.udp_port;
                    break;
                }
                delay(1);
            }
            Serial.println("Starting video stream.");

            cameraSource.begin();
            return true;
        }

        inline void loop() {
            camera_fb_t* frame = cameraSource.getFrame();
            if (frame) {
                send(frame);
                cameraSource.returnFrame(frame);
            }
        }
};

#endif