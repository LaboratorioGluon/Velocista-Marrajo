#include "comms/ota.h"
#include <esp_timer.h>
#include <esp_http_client.h>
#include <nvs_flash.h>
#include <esp_https_ota.h>
#include <esp_log.h>

#include "HwDevices.h"
#include "comms/commander.h"

static char TAG[] = "OTA";

void updateOTA()
{
    ESP_LOGI(TAG, "Starting OTA example task");
    esp_http_client_config_t config = {
        .url = "http://192.168.1.133:8000/firmware.bin?", // Cambialo por la URL donde estará tu binario compilado
        .keep_alive_enable = true,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };

    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = esp_https_ota(&ota_config);
    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        esp_restart();
    }
    else
    {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

uint8_t commandRunning; // If 0 : Do normal execution, otherwise, dont do the main loop
static Commander commander;


void doCommand()
{

    char lastCommand[50];
    commander.updateCommand();
    commander.getLastCommand(lastCommand, 50);

    if (!strcmp(lastCommand, "UPDATE_FW"))
    {
        ESP_LOGE(TAG, "NEW VERSION!!!");
        status.setStatus(OTA_UPDATE);
        
        commandRunning = 1;
        //motors.setDirection(MotorController::STOP);
        motor1.setPowerPercentage(0);
        motor2.setPowerPercentage(0);
        succion.setPowerPercentage(0);
        //sensors.stopAdc();
        vTaskDelay(pdMS_TO_TICKS(2000));
        updateOTA();
    }
}

void otaTask(void *arg)
{
    static uint64_t lastmicros = esp_timer_get_time();
    ESP_LOGE(TAG, "Iniciando OTA Task");
    while (true)
    {
        doCommand();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

TaskHandle_t otaTaskHandle;
void startOtaTask(UBaseType_t priority, const BaseType_t core)
{
    xTaskCreatePinnedToCore(otaTask, "OTA_Task", 4096, NULL, configMAX_PRIORITIES-2, &otaTaskHandle, core);
}
// Fin Micro FSM
