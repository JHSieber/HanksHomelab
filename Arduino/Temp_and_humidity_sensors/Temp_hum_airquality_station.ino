// DHT11 wiring: signal on D4
//CJMCU wiring: SDA on D5, SCL on D18, WAKE on gnd
//light sensor attach to D34

#include<iostream>
#include <Wire.h>
#include <SPI.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include "Adafruit_CCS811.h"
// #include <WiFi.h>
// #include <IFTTTWebhook.h>
 
//TODO Setup MQTT

// const char server[] = "";
// const char* MY_SSID = "";
// const char* MY_PWD = "";
int LED_BUILTIN = 2;
int CoPin = 16;
int LumPin = 17;
float CoValue = 0;
float LumValue = 0;
int analog;
uint8_t DHTPin = 14;
DHT dht(DHTPin, DHT11);  
float Temp;
float Humi; 
float Co2;


Adafruit_CCS811 ccs;
// WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

  // WiFi.begin(MY_SSID, MY_PWD);
  delay(100);
  pinMode(DHTPin, INPUT);
  dht.begin();


Serial.begin(9600);
 
Serial.println("CCS811 test");
 
if(!ccs.begin()){
Serial.println("Failed to start sensor! Please check your wiring.");
while(1);
}

//calibrate temperature sensor
while(!ccs.available());
float temp = ccs.calculateTemperature();
ccs.setTempOffset(temp - 25.0);
}



void loop() {
  
  delay (2000);  
  
  CoValue = analogRead(CoPin)/*/1024*5.0*/;
  std::cout<<"the CO is : "<<CoValue<<std::endl;
    
  Temp = dht.readTemperature();
  Humi = dht.readHumidity();
  std::cout<<"the temperature is : "<<Temp<<std::endl;
  std::cout<<"the humidity is : "<<Humi<<std::endl;
  
  LumValue = analogRead(LumPin);
  std::cout<<"light level "<<ConvertLux(LumValue)<<" lux"<<std::endl; 
  
  ccs.readData();
  std::cout<<"the co2 is : "<<ccs.geteCO2()<<" ppm"<<std::endl;
  int Co2Value = ccs.geteCO2();


if(ccs.available()){
float temp = ccs.calculateTemperature();
if(!ccs.readData()){
Serial.print("CO2: ");
Serial.print(ccs.geteCO2());
Serial.print("ppm, TVOC: ");
Serial.print(ccs.getTVOC());
Serial.print("ppb Temp:");
Serial.println(temp);
}
else{
Serial.println("ERROR!");
while(1);
}
}
   
  // if (client.connect(server, 80)) {
  //   Serial.println("connected to server");
  //   WiFi.printDiag(Serial);

  //   String data = "Humi="
  //         +                        (String) Humi
  //         +  "&Temp="  +(String) Temp
  //         +  "&Co=" +(String) CoValue
  //         +  "&Co2="    +(String) Co2Value
  //         +  "&Lum="   +(String) LumValue;
  // }
  // else
  // {
  // Serial.println("connection to server failed");  
  // }

}
float ConvertLux(int raw)
{
  float Lux= raw * 5.0 / 4096;
  return pow (10,Lux);
}

