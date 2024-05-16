#include <EnergyMonitorSettingsService.h>

EnergyMonitorSettingsService::EnergyMonitorSettingsService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager) :
    _httpEndpoint(EnergyMonitorSettings::read,
                  EnergyMonitorSettings::update,
                  this,
                  server,
                  ENERGY_MONITOR_SETTINGS_PATH,
                  securityManager,
                  AuthenticationPredicates::IS_AUTHENTICATED),
    _fsPersistence(EnergyMonitorSettings::read, EnergyMonitorSettings::update, this, fs, ENERGY_MONITOR_SETTINGS_FILE) {
}

void EnergyMonitorSettingsService::begin() {
  _fsPersistence.readFromFS();
}