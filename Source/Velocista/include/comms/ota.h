#ifndef __OTA_H__
#define __OTA_H__

#include "freertos/FreeRTOS.h"

// CONFIG_ESP_HTTPS_OTA_ALLOW_HTTP=y

void doCommand();

void startOtaTask(UBaseType_t priority, const BaseType_t core);

#endif //__OTA_H__