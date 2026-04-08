// Sketch to use an ESP32 + esp32 camera to run a motion detection algorithm and save snapshots when motion is detected

#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "../secrets.h" 

// Target Unraid Server
const char* serverName = "http://192.168.xx.xxx:8080/";

// ========== PINS FOR ESP32-S CAM ==========
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Globals
WiFiClient client;
unsigned long lastTriggerTime = 0;
long lastFingerprint = 0;
const int motionThreshold = 2500;

void setup_wifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected! IP: " + WiFi.localIP().toString());
}

void setupCamera() {
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 15;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_DRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

  // 1. Initialize Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  // 2. Adjust Sensor Settings AFTER Init
  sensor_t * s = esp_camera_sensor_get();
  if (s) {
    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);
    s->set_exposure_ctrl(s, 0); 
    s->set_aec_value(s, 300); 
  }
  
  Serial.println("Camera Ready!");
}

// Function to compare previous frame with current frame to detect motion
bool isMotion(camera_fb_t * fb) {
  if (!fb || fb->len < 1000) return false;

  long currentFingerprint = 0;
  // Build up fingerprint from current frame buffer
  for (size_t i = 500; i < fb->len; i += 100) {
    currentFingerprint += fb->buf[i];
  }

  if (lastFingerprint == 0) {
    lastFingerprint = currentFingerprint;
    return false;
  }

  // compare new and old finger print with our threshold for motion
  long diff = abs(currentFingerprint - lastFingerprint);
  lastFingerprint = currentFingerprint;

  if (diff > motionThreshold) {
    Serial.printf("Motion Detected! Diff: %ld\n", diff);
    return true;
  }
  return false;
}

// Function to send the image to unraid server
void sendImage(camera_fb_t * fb) {
  HTTPClient http;
  http.begin(client, serverName);
  http.addHeader("Content-Type", "image/jpeg");
  
  int httpResponseCode = http.POST(fb->buf, fb->len);
  
  Serial.printf("Unraid Response: %d\n", httpResponseCode);
  http.end();
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  setupCamera();
}

void loop() {
  // get most recent frame buffer
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) return;

  // Check if motion occurred AND if we are out of the 5-second cooldown to prevent constant detections 
  if (isMotion(fb) && (millis() - lastTriggerTime > 5000)) {
    sendImage(fb); //Send image if motion was detected
    lastTriggerTime = millis();
  }

  esp_camera_fb_return(fb);
  delay(200); // 5 checks per second
}