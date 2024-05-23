#include <ota_helper.h>

ota_helper::ota_helper(ThingsBoard &tb, Espressif_Updater &updater)
    : _tb(tb), _updater(updater)
{
}

void ota_helper::updatedCallback(const bool& success)
{
    if (success){
        Serial.println("OTA update success");
        esp_restart();
    }
    else{
        Serial.println("OTA update failed");
        onGoingUpdate = false;
    }
}

void ota_helper::progressCallback(const size_t& currentChunk, const size_t& totalChuncks)
{
    onGoingUpdate = true;
    Serial.printf("Progress %.2f%%\n", static_cast<float>(currentChunk * 100U) / totalChuncks);
}

void ota_helper::checkForOTAUpdate()
{
    if(!currentFWSent){
        currentFWSent = _tb.Firmware_Send_Info(CURRENT_FIRMWARE_TITLE, CURRENT_FIRMWARE_VERSION) && _tb.Firmware_Send_State(FW_STATE_UPDATED);
    }
    if(!updateRequestSent){
        Serial.println("Firwmare Update Subscription...");
        
        auto progressCallbackFunc = std::bind(&ota_helper::progressCallback, this, std::placeholders::_1, std::placeholders::_2);
        auto updatedCallbackFunc = std::bind(&ota_helper::updatedCallback, this, std::placeholders::_1);

        // Serial.println(CURRENT_FIRMWARE_TITLE);
        // Serial.println(CURRENT_FIRMWARE_VERSION);

        // Serial.print("title address: ");
        // Serial.println((uintptr_t)CURRENT_FIRMWARE_TITLE, HEX);
        // Serial.print("version address: ");
        // Serial.println((uintptr_t)CURRENT_FIRMWARE_VERSION, HEX);

        static const OTA_Update_Callback callback( progressCallbackFunc,
                                            updatedCallbackFunc, 
                                            CURRENT_FIRMWARE_TITLE, 
                                            CURRENT_FIRMWARE_VERSION, 
                                            &_updater, 
                                            FIRMWARE_FAILURE_RETRIES,
                                            FIRMWARE_PACKET_SIZE
                                          );

        Serial.println(callback.Get_Firmware_Title());
        Serial.println(callback.Get_Firmware_Version());

        updateRequestSent = _tb.Subscribe_Firmware_Update(callback); // Subscribe and wait for a new firmware update
        // updateRequestSent = _tb.Start_Firmware_Update(callback);        // Immediately start the update 

        if(!updateRequestSent){
            Serial.println("Firwmare Update Subscription failed");
        }else{
            Serial.println("Firwmare Update Subscription success");
        }

    }
}

void ota_helper::setFirmwareData(char* title, char* version)
{
    strcpy(CURRENT_FIRMWARE_TITLE, title);
    strcpy(CURRENT_FIRMWARE_VERSION, version);
}