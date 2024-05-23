#ifndef OTA_HELPER_H
#define OTA_HELPER_H
#include <Arduino.h>
#include <ThingsBoard.h>
#include <Espressif_Updater.h>

class ota_helper
{
  private:
    ThingsBoard & _tb;
    Espressif_Updater & _updater;
    // Statuses for updating
    bool currentFWSent = false;
    bool updateRequestSent = false;

    char CURRENT_FIRMWARE_TITLE[15] = "";
    char CURRENT_FIRMWARE_VERSION[15] = "";
    uint8_t FIRMWARE_FAILURE_RETRIES = 12;
    uint16_t FIRMWARE_PACKET_SIZE = 16384;
  public:
    bool onGoingUpdate = false;
    ota_helper(ThingsBoard &tb, Espressif_Updater &updater);
    void setFirmwareData(char* title, char* version);
    void updatedCallback(const bool& success);
    void progressCallback(const size_t& currentChunk, const size_t& totalChuncks);
    void checkForOTAUpdate();

};



#endif
