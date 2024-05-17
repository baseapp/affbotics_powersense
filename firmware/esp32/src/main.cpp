#include <Arduino.h>
#include <SPI.h>
#include <ESP8266React.h>
#include "energyic_SPI.h"
#include <EnergyMonitorSettingsService.h>
#include <energyMonitor.h>
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>

constexpr char TOKEN[] = "v651bxv7eyzutp5izt1t"; // TODO: Dynamically generate token
constexpr char THINGSBOARD_SERVER[] = "iot.affbotics.com";
constexpr int THINGSBOARD_PORT = 1883;
constexpr uint16_t MAX_MESSAGE_SIZE = 512;

#define SERIAL_BAUD_RATE 115200

ATM90E26_SPI eic;
AsyncWebServer server(80);
ESP8266React esp8266React(&server);
// Initalize the Mqtt client instance
WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

// LightMqttSettingsService lightMqttSettingsService =
//     LightMqttSettingsService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
// LightStateService lightStateService = LightStateService(&server,
//                                                         esp8266React.getSecurityManager(),
//                                                         esp8266React.getMqttClient(),
//                                                         &lightMqttSettingsService);

EnergyMonitorSettingsService energyMonitorSettingsService =
      EnergyMonitorSettingsService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
EnergyMonitor energyMonitor = EnergyMonitor(&server, eic, &tb, &energyMonitorSettingsService);


// float importEnergy = 0;
// float exportEnergy = 0; 

void fetchLoop(void *pvParameters) {
  while (true) {
    energyMonitor.fetchEnergyData();
    delay(5000);
  }
}

void thingboardLoop(void *pvParameters) {
  while (true) {
    if(WiFi.status() != WL_CONNECTED){
      Serial.println("WiFi Disconnected");
      delay(1000);
      continue;
    }
    if(!tb.connected()){
      if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
        Serial.println("Failed to connect");
        delay(1000);
        continue;
      }else{
        Serial.println("Connected to ThingsBoard");
      }
    }

    tb.loop();
  }
}

void setup() {
  // start serial and filesystem
  Serial.begin(SERIAL_BAUD_RATE);

  // start the framework and demo project
  esp8266React.begin();

  energyMonitorSettingsService.begin();
  energyMonitor.begin();

  // start the server
  server.begin();

  // task to fetch energy data
  xTaskCreate(
              fetchLoop, /* Task function. */
              "TaskOne",  /* String with name of task. */
              10000,      /* Stack size in bytes. */
              NULL,       /* Parameter passed as input of the task */
              1,          /* Priority of the task. */
              NULL        /* Task handle. */
  );        
  xTaskCreate(
              thingboardLoop, /* Task function. */
              "TaskTwo",  /* String with name of task. */
              10000,      /* Stack size in bytes. */
              NULL,       /* Parameter passed as input of the task */
              1,          /* Priority of the task. */
              NULL        /* Task handle. */
  );
}



void loop() {
  // run the framework's loop function
  esp8266React.loop();

}
