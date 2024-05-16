#include <Arduino.h>
#include <SPI.h>
#include <ESP8266React.h>
#include "energyic_SPI.h"
#include <EnergyMonitorSettingsService.h>
#include <energyMonitor.h>

#define SERIAL_BAUD_RATE 115200

ATM90E26_SPI eic;
AsyncWebServer server(80);
ESP8266React esp8266React(&server);
// LightMqttSettingsService lightMqttSettingsService =
//     LightMqttSettingsService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
// LightStateService lightStateService = LightStateService(&server,
//                                                         esp8266React.getSecurityManager(),
//                                                         esp8266React.getMqttClient(),
//                                                         &lightMqttSettingsService);

EnergyMonitorSettingsService energyMonitorSettingsService =
      EnergyMonitorSettingsService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
EnergyMonitor energyMonitor = EnergyMonitor(&server, eic, esp8266React.getMqttClient(), &energyMonitorSettingsService);


// float importEnergy = 0;
// float exportEnergy = 0; 

void fetchLoop(void *pvParameters) {
  while (true) {
    energyMonitor.fetchEnergyData();
    delay(5000);
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
}



void loop() {
  // run the framework's loop function
  esp8266React.loop();

}
