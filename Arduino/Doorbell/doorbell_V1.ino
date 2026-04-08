// Doorbell imitation device
// Once 'doorbell' is pressed on breadboard, publish to MQTT so Home assistant can play doorbell sounds on my computer

#include "../../secrets.h"
#include <WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

// constants
const int buttonPin = 33;  
const int ledPin = 25; 

// --- MQTT Settings ---
const int mqtt_port = 1883;
const char* mqtt_topic = "home/doorbell";

WiFiClient espClient;
PubSubClient client(espClient);

int buttonState = 0; 
int lastButtonState = HIGH;

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
    if (retries > 20) { // After 10 seconds give up
      Serial.println("Failed to connect to WiFi!");
      retries = 0;
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


void setup_OTA() {
  // OTA Setup      
  ArduinoOTA.setHostname(DOORBELL_OTA_HOSTNAME);
  ArduinoOTA.setPassword(DOORBELL_OTA_PASSWORD); 

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
  Serial.begin(115200);
   // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT_PULLUP);

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

  // Read the state 
  buttonState = digitalRead(buttonPin);

  // Detect a press, LOW = Pressed 
  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("Button Pressed");
    digitalWrite(ledPin, HIGH);

    //Publish to MQTT for Homeassistant to hear
    client.publish(mqtt_topic, "pressed");
  }
  else if (buttonState == HIGH && lastButtonState == LOW) {
    // Button released
    digitalWrite(ledPin, LOW);
    client.publish(mqtt_topic, "unpressed");
  }

  // Save the current state for next loop
  lastButtonState = buttonState;
}
