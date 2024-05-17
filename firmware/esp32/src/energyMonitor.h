#ifndef ENERGY_MONITOR_H
#define ENERGY_MONITOR_H

#include <EnergyMonitorSettingsService.h>

#include <HttpEndpoint.h>
#include <WebSocketTxRx.h>
#include "energyic_SPI.h"
#include <ArduinoJson.h>
#include <ThingsBoard.h>

#define ENERGY_DATA_ENDPOINT_PATH "/rest/energyMonitor"

class EnergyMonitor{
  public:
    float voltage;
    float current;
    float activePower;
    float frequency;
    float powerFactor;
    float importEnergy;
    float exportEnergy;
    String mqttPath;
    String energyJsonString;

    EnergyMonitor(AsyncWebServer* server,
                  ATM90E26_SPI eic,
                  ThingsBoard* tb,
                  EnergyMonitorSettingsService* energyMonitorSettingsService);
    void begin();
    void fetchEnergyData();
  private:
    ATM90E26_SPI _eic;
    ThingsBoard* _tb;
    EnergyMonitorSettingsService* _energyMonitorSettingsService;

    void registerConfig();
    void onConfigUpdated();
    void energyDataEndpoint(AsyncWebServerRequest* request);

};


#endif