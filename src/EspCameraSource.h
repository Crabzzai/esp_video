#ifndef ESP_CAMERA_SOURCE_H
#define ESP_CAMERA_SOURCE_H

#include "esp_camera.h"

// Pins for AI Thinker CAM Board
#define PWDN_GPIO_NUM   32
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM   0
#define SIOD_GPIO_NUM   26
#define SIOC_GPIO_NUM   27

#define Y9_GPIO_NUM     35
#define Y8_GPIO_NUM     34
#define Y7_GPIO_NUM     39
#define Y6_GPIO_NUM     36
#define Y5_GPIO_NUM     21
#define Y4_GPIO_NUM     19
#define Y3_GPIO_NUM     18
#define Y2_GPIO_NUM     5

#define VSYNC_GPIO_NUM  25
#define HREF_GPIO_NUM   23
#define PCLK_GPIO_NUM   22

// 4 for flash led or 33 for normal led
#define LED_GPIO_NUM    4

#define XCLK_FREQ_HZ    20000000

class EspCameraSource {
    public:
        inline bool begin(framesize_t frame_size = FRAMESIZE_QVGA, int quality = 12) {
            camera_config_t config;
            config.ledc_channel = LEDC_CHANNEL_0;
            config.ledc_timer = LEDC_TIMER_0;
            config.pin_d0 = Y2_GPIO_NUM;
            config.pin_d1 = Y3_GPIO_NUM;
            config.pin_d2 = Y4_GPIO_NUM;
            config.pin_d3 = Y5_GPIO_NUM;
            config.pin_d4 = Y6_GPIO_NUM;
            config.pin_d5 = Y7_GPIO_NUM;
            config.pin_d6 = Y8_GPIO_NUM;
            config.pin_d7 = Y9_GPIO_NUM;
            config.pin_xclk = XCLK_GPIO_NUM;
            config.pin_pclk = PCLK_GPIO_NUM;
            config.pin_vsync = VSYNC_GPIO_NUM;
            config.pin_href = HREF_GPIO_NUM;
            config.pin_sccb_sda = SIOD_GPIO_NUM;
            config.pin_sccb_scl = SIOC_GPIO_NUM;
            config.pin_pwdn = PWDN_GPIO_NUM;
            config.pin_reset = RESET_GPIO_NUM;
            config.xclk_freq_hz = XCLK_FREQ_HZ;
            config.frame_size = frame_size;
            config.pixel_format = PIXFORMAT_JPEG;
            config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
            config.fb_location = CAMERA_FB_IN_PSRAM;
            config.jpeg_quality = quality;
            config.fb_count = 2;

            return (esp_camera_init(&config) == ESP_OK);
        }

        inline camera_fb_t* getFrame() {
            return esp_camera_fb_get();
        }

        inline void returnFrame(camera_fb_t* fb) {
            if (fb) esp_camera_fb_return(fb);
        }
};

#endif