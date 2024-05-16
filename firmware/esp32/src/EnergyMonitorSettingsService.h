#ifndef ENERGY_MONITOR_SETTINGS_SERVICE_H
#define ENERGY_MONITOR_SETTINGS_SERVICE_H

#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <SettingValue.h>

#define ENERGY_MONITOR_SETTINGS_FILE "/config/brokerSettings.json"
#define ENERGY_MONITOR_SETTINGS_PATH "/rest/brokerSettings"

class EnergyMonitorSettings {
 public:
  String mqttPath;
  String name;
  String uniqueId;

  static void read(EnergyMonitorSettings& settings, JsonObject& root) {
    root["mqtt_path"] = settings.mqttPath;
    root["name"] = settings.name;
    root["unique_id"] = settings.uniqueId;
  }

  static StateUpdateResult update(JsonObject& root, EnergyMonitorSettings& settings) {
    settings.mqttPath = root["mqtt_path"] | SettingValue::format("energy/#{unique_id}");
    settings.name = root["name"] | SettingValue::format("energy-#{unique_id}");
    settings.uniqueId = root["unique_id"] | SettingValue::format("energy-#{unique_id}");
    return StateUpdateResult::CHANGED;
  }
};

class EnergyMonitorSettingsService : public StatefulService<EnergyMonitorSettings> {
 public:
  EnergyMonitorSettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);
  void begin();

 private:
  HttpEndpoint<EnergyMonitorSettings> _httpEndpoint;
  FSPersistence<EnergyMonitorSettings> _fsPersistence;
};



#endif  // end ENERGY_MONITOR_SETTINGS_SERVICE_H