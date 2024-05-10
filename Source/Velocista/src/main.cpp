#include <nvs_flash.h>
#include <driver/gpio.h>
#include "driver/adc.h"
#include <BtRobotController.h>
#include <driver/ledc.h>
#include "esp_log.h"

#include "comms/BTCommands.h"
#include "comms/wifi.h"
#include "comms/ota.h"

#include "HwDevices.h"
#include "drivers/led_strip_encoder.h"
#include "driver/rmt_tx.h"


const char * TAG = "MAIN";

/***** EJEMPLO WS2812 ******/
#define EXAMPLE_LED_NUMBERS         2
#define EXAMPLE_CHASE_SPEED_MS      10

static uint8_t led_strip_pixels[EXAMPLE_LED_NUMBERS * 3];

extern "C" void app_main();



char waitForChar(){
    char b;
    do{
        b = getchar();
        vTaskDelay(pdMS_TO_TICKS(100));
    }while(b == 0xFF);
    return b;
}



TaskHandle_t statusTask;

void taskLoop(void *arg)
{
    while(1)
    {
        status.loop(100);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

TaskHandle_t morroTask;

void taskMorro(void *arg)
{
    uint64_t startTime=esp_timer_get_time();
    uint64_t stopTime = 0;
    while(1)
    {
        
        printf("%ld\n", morro.getLinePosition());
#if 0
        startTime=esp_timer_get_time();
        morro.getLinePosition();
        stopTime = esp_timer_get_time();
        printf("Delta: %llu\n", stopTime-startTime);
        startTime = stopTime;
#endif

    }
}

void app_main() {

    nvs_flash_init();
    leds.init();
    status.setLedController(&leds);
    status.setStatus(OTA_AND_BT_OK);
    xTaskCreatePinnedToCore(taskLoop, "Status_Task", 4096, NULL, 9, &statusTask, 0);
    wifi_init_sta();
    startOtaTask(1, 0);
    robotCtrl.Init("MARRAJO", robotConfig, 3);
    phyinputs.init();
    
    #if 0
        //vTaskDelay(pdMS_TO_TICKS(0));
        motor1.Init(250, 125, 250);
        motor2.Init(250, 125, 250);
        printf("Connect the ESC now!\n");
        printf("Wait for 2 beeps from the ESC and press a key\n");
        waitForChar();
        motor1.setPowerPercentage(0);
        motor2.setPowerPercentage(0);
        printf("Wait for 1,2,3 beeps \n");
        waitForChar();
    #else
        motor1.Init(125,125,250);
        motor2.Init(125,125,250);
        vTaskDelay(pdMS_TO_TICKS(5000));
    #endif 
    
    printf("Ready!\n");
    status.setStatus(MOTORS_OK);
    morro.init();

    status.setStatus(CALIBRATING);
    while(morro.calibrate() == 0);

    status.setStatus(WAITING_START);
    while(phyinputs.getInput(0) == 1)
    {
        morro.calibrate();
    }
    status.setStatus(MOTORS_OK);

    xTaskCreatePinnedToCore(taskMorro, "Morro_Task", 4096, NULL, 9, &morroTask, 1);
    uint8_t isWifiInit = 0;
    while(1)
    {
/*
        if(phyinputs.getInput(1) == 0 && isWifiInit == 0)
        {
            status.setStatus(OTA_AND_BT_OK);
            wifi_init_sta();
            startOtaTask(1, 0);
            status.setStatus(MOTORS_OK);
            isWifiInit = 1;
        }*/
        vTaskDelay(100);
    }
}