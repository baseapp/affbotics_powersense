#include <Arduino.h>
#include <SPI.h>
#include <ESP8266React.h>
#include <LightMqttSettingsService.h>
#include <LightStateService.h>
#include "energyic_SPI.h"

#define SERIAL_BAUD_RATE 115200

ATM90E26_SPI eic;
AsyncWebServer server(80);
ESP8266React esp8266React(&server);
LightMqttSettingsService lightMqttSettingsService =
    LightMqttSettingsService(&server, esp8266React.getFS(), esp8266React.getSecurityManager());
LightStateService lightStateService = LightStateService(&server,
                                                        esp8266React.getSecurityManager(),
                                                        esp8266React.getMqttClient(),
                                                        &lightMqttSettingsService);
float importEnergy = 0;
float exportEnergy = 0;

void fetchEnergyData() {
  float voltage = eic.GetLineVoltage();
  yield();
  float current = eic.GetLineCurrent() * 3;
  yield();
  float activePower = eic.GetActivePower() * 3;
  yield();
  float frequency = eic.GetFrequency();
  yield();
  float powerFactor = eic.GetPowerFactor();
  yield();
  float importEnergyTemp = eic.GetImportEnergy();
  importEnergy += importEnergyTemp;
  yield();
  float exportEnergyTemp = eic.GetExportEnergy();
  exportEnergy += exportEnergyTemp;
  yield();
  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.print("Current: ");
  Serial.println(current);
  Serial.print("Active Power: ");
  Serial.println(activePower);
  Serial.print("Frequency: ");
  Serial.println(frequency);
  Serial.print("Power Factor: ");
  Serial.println(powerFactor);
  Serial.print("Import Energy: ");
  Serial.println(importEnergy);
  Serial.print("Export Energy: ");
  Serial.println(exportEnergy);
  Serial.println(" ");

}

void fetchLoop(void *pvParameters) {
  while (true) {
    fetchEnergyData();
    delay(1000);
  }
}

void setup() {
  // start serial and filesystem
  Serial.begin(SERIAL_BAUD_RATE);

  // start the framework and demo project
  esp8266React.begin();

  // load the initial light settings
  lightStateService.begin();

  // start the light service
  lightMqttSettingsService.begin();

  // start the server
  server.begin();

  eic.SetUGain(26548);
  eic.SetIGain(50416); // divide by 3
  eic.SetCRC1(0x41FB);
  eic.SetCRC2(0xA274); //0xB177
  eic.InitEnergyIC();

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
