#include <Arduino.h>
#include <SPI.h>
#define THINGSBOARD_ENABLE_PROGMEM 0
#define THINGSBOARD_ENABLE_DEBUG 1
#include <ESP8266React.h>
#include "energyic_SPI.h"
#include <EnergyMonitorSettingsService.h>
#include <energyMonitor.h>
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>
#include <ota_helper.h>

char FIRMWARE_TITLE[] = "ESP32";
char FIRMWARE_VERSION[] = "1.0.0";

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
Espressif_Updater updater;
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);
ota_helper otaHelper(tb, updater);

EnergyMonitorSettingsService energyMonitorSettingsService =
      EnergyMonitorSettingsService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
EnergyMonitor energyMonitor = EnergyMonitor(&server, eic, &tb, &energyMonitorSettingsService);

void fetchLoop(void *pvParameters) {
  while (true) {
    energyMonitor.fetchEnergyData();
    if(!otaHelper.onGoingUpdate){
      // Publish energy data to ThingsBoard only if there is no ongoing update
      energyMonitor.publishEnergyData();
    }
    delay(10000);
  }
}

void thingboardLoop(void *pvParameters) {
  while (true) {
    if(WiFi.status() != WL_CONNECTED){
      // Serial.println("WiFi Disconnected");
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

    otaHelper.checkForOTAUpdate();
    tb.loop();
    delay(1000);
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

  otaHelper.setFirmwareData(FIRMWARE_TITLE, FIRMWARE_VERSION);

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
