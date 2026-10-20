#ifndef ESP_SHARED_CONFIG_H
#define ESP_SHARED_CONFIG_H

#include <Arduino.h>

struct Command {
    uint8_t command_id;
    uint16_t udp_port;
};

#define DISCOVERY_PORT 5000

#endif