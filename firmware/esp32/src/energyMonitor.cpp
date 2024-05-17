#include <energyMonitor.h>

EnergyMonitor::EnergyMonitor(AsyncWebServer* server,
                             ATM90E26_SPI eic,
                             ThingsBoard* tb,
                             EnergyMonitorSettingsService* energyMonitorSettingsService) :
    _eic(eic),
    _tb(tb),
    _energyMonitorSettingsService(energyMonitorSettingsService)
{
  server->on(ENERGY_DATA_ENDPOINT_PATH, HTTP_GET, std::bind(&EnergyMonitor::energyDataEndpoint, this, std::placeholders::_1));

}


void EnergyMonitor::registerConfig() {

}

void EnergyMonitor::begin() {
  _eic.SetUGain(26548);
  _eic.SetIGain(50416); // divide by 3
  _eic.SetCRC1(0x41FB);
  _eic.SetCRC2(0xA274); //0xB177
  _eic.InitEnergyIC();
}

void EnergyMonitor::fetchEnergyData() {

  bool noLoad = false;
  float voltage = 0;
  float current = 0;
  float activePower = 0;
  float frequency = 0;
  float powerFactor = 0;
  float importEnergyTemp = 0;
  float exportEnergyTemp = 0;  

  unsigned short meterStatus = _eic.GetMeterStatus();
  Serial.print("Meter Status:");
  Serial.println(meterStatus,HEX);

  unsigned short bit14 = (meterStatus >> 13) & 0x1;
  unsigned short bit15 = (meterStatus >> 14) & 0x1;
  
  if(bit14 == 1 && bit15 == 1){
    Serial.println("No load");
    noLoad = true;
  }

  yield();
  voltage = _eic.GetLineVoltage();
  yield();
  frequency = _eic.GetFrequency();
  yield();
  if(!noLoad){
    current = _eic.GetLineCurrent() * 3;
    yield();
    activePower = _eic.GetActivePower() * 3;
    yield();
    powerFactor = _eic.GetPowerFactor();
    yield();
    importEnergyTemp = _eic.GetImportEnergy();
    importEnergy += importEnergyTemp;
    yield();
    exportEnergyTemp = _eic.GetExportEnergy();
    exportEnergy += exportEnergyTemp;
    yield();
  }

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

  StaticJsonDocument<JSON_OBJECT_SIZE(7) + 140> jsonBuffer;
  // Fill in the JSON object with data
  jsonBuffer["voltage"] = voltage;
  jsonBuffer["current"] = current;
  jsonBuffer["power"] = activePower;
  jsonBuffer["frequency"] = frequency;
  jsonBuffer["powerFactor"] = powerFactor;
  jsonBuffer["importEnergy"] = importEnergy;
  jsonBuffer["exportEnergy"] = exportEnergy;

  // Serialize the JSON object
  energyJsonString = "";
  serializeJson(jsonBuffer, energyJsonString);

  // Publish the serialized JSON data
  // _mqttClient->publish("v1/devices/me/telemetry", 0, false, energyJsonString.c_str());
  if(_tb->connected()){
    _tb->sendTelemetryJson(energyJsonString.c_str());
  }

  // _mqttClient->publish((mqttPath + "/v").c_str(), 0, false, String(voltage).c_str());
  // _mqttClient->publish((mqttPath + "/i").c_str(), 0, false, String(current).c_str());
  // _mqttClient->publish((mqttPath + "/p").c_str(), 0, false, String(activePower).c_str());
  // _mqttClient->publish((mqttPath + "/f").c_str(), 0, false, String(frequency).c_str());
  // _mqttClient->publish((mqttPath + "/pf").c_str(), 0, false, String(powerFactor).c_str());
  // _mqttClient->publish((mqttPath + "/ie").c_str(), 0, false, String(importEnergy).c_str());
  // _mqttClient->publish((mqttPath + "/ee").c_str(), 0, false, String(exportEnergy).c_str());

}

void EnergyMonitor::energyDataEndpoint(AsyncWebServerRequest* request) {
  request->send(200, "application/json", energyJsonString);
}