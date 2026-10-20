#ifndef ESP_RECEIVER_H
#define ESP_RECEIVER_H

#include <WiFi.h>
#include <WiFiUdp.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>

TFT_eSPI tft = TFT_eSPI();

inline bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= tft.height()) return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

class EspReceiver {
    private:
        WiFiUDP _udp;
        uint8_t* _jpeg_buffer;
        size_t _current_frame_size;
        size_t _received_bytes;
        static const size_t MAX_FRAME_SIZE = 35000;
        bool _has_psram;
        uint16_t _udp_port;
        const char* _ssid;
        const char* _password;
        IPAddress _ap_ip;
        IPAddress _subnet;
        IPAddress _broadcast_ip;
        unsigned long _last_broadcast;

    public:
        inline bool begin(uint16_t port, const char* ssid, const char* password) {
            _udp_port = port;
            _ssid = ssid;
            _password = password;
            _has_psram = psramFound();
            if (!_has_psram) {
                Serial.println("PSRAM not found, using internal memory for JPEG buffer.");
                _jpeg_buffer = (uint8_t*)malloc(MAX_FRAME_SIZE);
                if (!_jpeg_buffer) {
                    Serial.println("Failed to allocate memory for JPEG buffer.");
                    return false;
                }
            } else {
                Serial.println("PSRAM found, using PSRAM for JPEG buffer.");
                _jpeg_buffer = (uint8_t*)ps_malloc(MAX_FRAME_SIZE);
                if (!_jpeg_buffer) {
                    Serial.println("Failed to allocate PSRAM for JPEG buffer.");
                    return false;
                }
            }

            tft.begin();
            tft.setRotation(1);
            tft.fillScreen(TFT_BLACK);

            TJpgDec.setJpgScale(1);
            TJpgDec.setSwapBytes(true);
            TJpgDec.setCallback(tft_output);

            WiFi.mode(WIFI_AP);
            WiFi.softAP(_ssid, _password);
            Serial.print("AP IP address: ");
            Serial.println(WiFi.softAPIP());

            _udp.begin(_udp_port);
            Serial.printf("Listening for UDP packets on port %d\n", _udp_port);

            _current_frame_size = 0;
            _received_bytes = 0;

            _ap_ip = WiFi.softAPIP();
            _subnet = WiFi.softAPSubnetMask();
            
            for (int i = 0; i < 4; i++) {
                _broadcast_ip[i] = _ap_ip[i] | ~_subnet[i];
            }

            Serial.print("Dynamic Broadcast IP: ");
            Serial.println(_broadcast_ip);
            return true;
        }

        inline void loop() {
            int packetSize = _udp.parsePacket();
            if (packetSize > 6) { //larger than the header
                uint8_t packet[1472];
                _udp.read(packet, packetSize);

                uint16_t chunk_id = *((uint16_t)packet);
                size_t total_len = *((size_t*)(paclet + 2));
                size_t payload_size = packetSize - 6;

                if (chunk_id == 0) {
                    _received_bytes = 0;
                    _current_frame_size = total_len;
                }

                if (_received_bytes + payload_size <= MAX_FRAME_SIZE) {
                    memcpy(_jpeg_buffer + _received_bytes, packet + 6, payload_size);
                    _received_bytes += payload_size;
                }

                if (_received_bytes == _current_frame_size && _current_frame_size > 0) {
                    TJpgDec.drawJpg(0, 0, _jpeg_buffer,_current_frame_size);
                    _received_bytes = 0;
                }
            }

            if (millis() % 2000 < 10) {
                Command cmd = {0x01, udpPort};

                _udp.beginPacket(_broadcast_ip, _udp_port);
                _udp.write((uint8_t*)&cmd, sizeof(cmd));
                _udp.endPacket();

                _last_broadcast = millis();
            }
        }
}

#endif