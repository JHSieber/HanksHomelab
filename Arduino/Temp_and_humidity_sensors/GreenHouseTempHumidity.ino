// Greenhouse Temperature and humidity sensor
// Sketch to get data from dht22 sensor, connect to wifi, and send the info to home assistant through MQTT broker

#include "../secrets.h"
#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>


// MQTT Settings
const int mqtt_port = 1883;
const char* mqtt_topic = "home/esp32/temperature";

//  DHT Setup 
#define DHTPIN 14          // GPIO14 = DHT22 temp sensor
#define DHTTYPE DHT22      // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// MQTT Topics for our two bits of info we are sending
const char* temperature_topic = "home/esp32/temperature/greenhouse";
const char* humidity_topic = "home/esp32/humidity/greenhouse";


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA); //Station mode
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retries++;
    if (retries > 20) { // try for 10 seconds
      Serial.println("Failed to connect to WiFi!");
      return;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until reconnected
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


void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // OTA Setup
  ArduinoOTA.setHostname(GREENHOUSE_OTA_HOSTNAME);
  ArduinoOTA.setPassword(GREENHOUSE_OTA_PASSWORD); 

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

  client.setServer(MQTT_SERVER, 1883);
}

void loop() {
  ArduinoOTA.handle(); 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read DHT22 valuesj 
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(true); // Celsius by default

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Debug prints
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °F  |  Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Publish to MQTT
  client.publish(temperature_topic, String(temperature).c_str(), true);
  client.publish(humidity_topic, String(humidity).c_str(), true);

  delay(30000); // Read and publish every 30 seconds
}
