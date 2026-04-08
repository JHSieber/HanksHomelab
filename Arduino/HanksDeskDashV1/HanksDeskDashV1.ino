// Hank Desk Dash V1
// beginning code for a desk dashboard to control my smarthome and view real time info
// With an ESP32, an oled display, temp/humidity sensor, and basic controls.
// Connects to wifi and displays info on screen and sends commands back to Home assistant through MQTT

#include "../secrets.h"
#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <U8g2lib.h>



//  MQTT Settings 
const int mqtt_port = 1883; // default port
const char* mqtt_topic = "home/esp32/temperature";

//  DHT Setup 
#define DHTPIN 14          // GPIO14 DHT22 is 
#define DHTTYPE DHT22      // DHT 22 (AM2302)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// SPI OLED pin definitions
#define OLED_DC   17
#define OLED_CS   5
#define OLED_RESET 16

// Constructor for SH1106 128x64 SPI
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ OLED_CS, /* dc=*/ OLED_DC, /* reset=*/ OLED_RESET);


DHT dht(DHTPIN, DHTTYPE);

//  MQTT Topics 
const char* temperature_topic = "home/esp32/temperature/hanksdesk";
const char* humidity_topic = "home/esp32/humidity/hanksdesk";


WiFiClient espClient; 
PubSubClient client(espClient);

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA); // Station mode
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    // Show "Connecting..." screen with dot animation
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 12, "Connecting to WiFi");
    u8g2.drawStr(0, 28, WIFI_SSID);

    // Animate dots based on retry count
    char dots[4] = "   ";
    for (int i = 0; i < (retries % 4); i++) dots[i] = '.';
    u8g2.drawStr(0, 44, dots);

    u8g2.sendBuffer();

    delay(500);
    Serial.print(".");
    retries++;
    if (retries > 20) { // After 10 seconds give up
      Serial.println("Failed to connect to WiFi!");

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 12, "WiFi Failed!");
      u8g2.drawStr(0, 28, "Check credentials");
      u8g2.sendBuffer();

      return;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Show "Connected!" screen with IP address
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 12, "WiFi Connected!");
  u8g2.drawStr(0, 28, WIFI_SSID);

  // Display IP address
  char ipStr[20];
  WiFi.localIP().toString().toCharArray(ipStr, sizeof(ipStr));
  u8g2.drawStr(0, 44, ipStr);

  // Show current temp/humidity on connected screen if sensor is ready
  float h = dht.readHumidity();
  float t = dht.readTemperature(true); // Fahrenheit
  if (!isnan(h) && !isnan(t)) {
    char sensorStr[32];
    snprintf(sensorStr, sizeof(sensorStr), "%.1fF  %.1f%%RH", t, h);
    u8g2.drawStr(0, 58, sensorStr);
  }

  u8g2.sendBuffer();
  delay(2000); // Hold the connected screen for 2 seconds
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup_OTA() {
  // OTA Setup      
  ArduinoOTA.setHostname(HANKDESKDASH_OTA_HOSTNAME);
  ArduinoOTA.setPassword(HANKDESKDASH_OTA_PASSWORD); 

  ArduinoOTA.onStart([]() {
    Serial.println("Start updating...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready"); 
}


void setup() {
  Serial.begin(115200); //Serial setup

  // Init display first so we can give feedback on wifi status
  u8g2.begin();

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 12, "Hank's Desk");
  u8g2.drawStr(0, 28, "Starting up...");
  u8g2.sendBuffer();

  dht.begin();     
  delay(2000);      

  setup_wifi(); //Call wifi setup
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  setup_OTA(); //Call OTA setup

  client.setServer(MQTT_SERVER, 1883); //Set MQTT server
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read DHT22 values
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true); // Fahrenheit

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 12, "Sensor Error!");
    u8g2.drawStr(0, 28, "DHT22 read failed");
    u8g2.sendBuffer();

    return;
  }

  // Debug prints
  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.print(" °F  |  Humidity: ");
  // Serial.print(humidity);
  // Serial.println(" %");

  // Publish to MQTT
  client.publish(temperature_topic, String(temperature).c_str(), true);
  client.publish(humidity_topic, String(humidity).c_str(), true);

  // Update display with current readings
  char tempStr[16];
  char humStr[16];
  char ipStr[20];
  snprintf(tempStr, sizeof(tempStr), "Temp: %.1f F", temperature);
  snprintf(humStr, sizeof(humStr), "Hum:  %.1f %%", humidity);
  WiFi.localIP().toString().toCharArray(ipStr, sizeof(ipStr));

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 12, "Hank's Desk");
  u8g2.drawStr(0, 28, tempStr);
  u8g2.drawStr(0, 44, humStr);
  u8g2.drawStr(0, 58, ipStr);
  u8g2.sendBuffer();

  delay(30000); // Read and publish every 30 seconds
}
