#ifndef ENERGY_MONITOR_H
#define ENERGY_MONITOR_H

#include <EnergyMonitorSettingsService.h>

#include <HttpEndpoint.h>
#include <MqttPubSub.h>
#include <WebSocketTxRx.h>
// #include <SPI.h>
#include "energyic_SPI.h"
#include <ArduinoJson.h>

#define ENERGY_DATA_ENDPOINT_PATH "/rest/energyMonitor"

class EnergyMonitor{
  public:
    float voltage;
    float current;
    float activePower;
    float reactivePower;
    float frequency;
    float powerFactor;
    float importEnergy;
    float exportEnergy;
    String mqttPath;
    String energyJsonString;

    EnergyMonitor(AsyncWebServer* server,
                  ATM90E26_SPI eic,
                  AsyncMqttClient* mqttClient,
                  EnergyMonitorSettingsService* energyMonitorSettingsService);
    void begin();
    void fetchEnergyData();
  private:
    ATM90E26_SPI _eic;
    AsyncMqttClient* _mqttClient;
    EnergyMonitorSettingsService* _energyMonitorSettingsService;

    void registerConfig();
    void onConfigUpdated();
    void energyDataEndpoint(AsyncWebServerRequest* request);
    

};


#endif