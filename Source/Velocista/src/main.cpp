#include <nvs_flash.h>
#include <driver/gpio.h>
#include "driver/adc.h"
#include <BtRobotController.h>
#include <driver/ledc.h>
#include "esp_log.h"

#include "comms/BTCommands.h"
#include "comms/wifi.h"
#include "comms/ota.h"

#include "common/pid.h"
#include "common/log.h"

#include "HwDevices.h"
#include "drivers/led_strip_encoder.h"
#include "driver/rmt_tx.h"

#include "comms/udp_serial.h"

const char *TAG = "MAIN";

/***** EJEMPLO WS2812 ******/
#define EXAMPLE_LED_NUMBERS 2
#define EXAMPLE_CHASE_SPEED_MS 10

static uint8_t led_strip_pixels[EXAMPLE_LED_NUMBERS * 3];

extern "C" void app_main();

PID motorPid(0.1f, 0, 0);
UdpSerial uSerial("192.168.1.133", 5005);

char waitForChar()
{
    char b;
    do
    {
        b = getchar();
        vTaskDelay(pdMS_TO_TICKS(100));
    } while (b == 0xFF);
    return b;
}

TaskHandle_t statusTask;

void taskLoop(void *arg)
{
    while (1)
    {
        status.loop(100);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

TaskHandle_t morroTask;

float meanspeed = 40.0f;
float targetSpeed = 65.0f;
uint8_t stopOrder = 0;
void taskMorro(void *arg)
{

    int32_t motorR = 0;
    int32_t motorL = 0;
    uint64_t startTime = 0;
    uint64_t dt = 1;
    int32_t pidOutput;
    int32_t linePos = 0;
    int32_t newLinePos = 0;
    MORRO_RET lineStatus = MORRO_LINE_OK;
    struct logInfo info;
    uSerial.printf("Starting!!!\n");
    while (1)
    {

        if (status.getStatus() == STATUS::OTA_UPDATE)
        {
            vTaskDelay(100);
            continue;
        }

        lineStatus = morro.getLinePosition(newLinePos);

        if ( lineStatus == MORRO_LINE_LOST || stopOrder == 1)
        {
            // Stop all
            motor1.setPowerPercentage(0);
            motor2.setPowerPercentage(0);
            succion.setPowerPercentage(0);
            status.setStatus(SECURITY_STOP);
            dumpDataToUdp(uSerial);
            stopOrder = 0;
            while(1)
            {
                status.setStatus(WAITING_START);   
                while(phyinputs.getInput(1) == 1)
                {
                    vTaskDelay(100);
                }
                linePos = 0;
                startTime = 0;
                meanspeed = 40.0f;
                resetLog();
                uSerial.printf("reStarting!!!\n");
                printf("ReStarting!\n");
                status.setStatus(NOMINAL);
                vTaskDelay(pdMS_TO_TICKS(1000));
                break;
            }
        }

        linePos = 0.9 * newLinePos + 0.1 * linePos;

        // printf("%ld\n", linePos);
        if (startTime != 0)
        {
            dt = esp_timer_get_time() - startTime;
            pidOutput = (int32_t)motorPid.update(linePos, dt/1000.0f);
        }
        else
        {
            pidOutput = 0;
        }
        startTime = esp_timer_get_time();

        motorR = 0.8 * (meanspeed + pidOutput) + 0.2 * motorR;
        motorL = 0.8 * (meanspeed - pidOutput) + 0.2 * motorR;

        motorR = (motorR > 80) ? 80 : motorR;
        motorL = (motorL > 80) ? 80 : motorL;

        motorR = (motorR < 0) ? 0 : motorR;
        motorL = (motorL < 0) ? 0 : motorL;

        motor1.setPowerPercentage(motorR);
        motor2.setPowerPercentage(motorL);

        if (meanspeed < targetSpeed)
        {
            meanspeed += dt * 10 / 1000000.0f;
        }

        info.linePos = linePos;
        info.pidOutput = pidOutput;
        info.motorR = motorR;
        info.motorL= motorL;
        info.dt = dt;
        storeLoopInfo(info);
        //uSerial.printf("%ld %ld %ld %llu\n", linePos, motorR, motorL, dt);
        // printf("(%ld) %-5ld %-5ld\n",pidOutput, motorR, motorL);

        // vTaskDelay(2);

#if 0
        startTime=esp_timer_get_time();
        morro.getLinePosition();
        stopTime = esp_timer_get_time();
        printf("Delta: %llu\n", stopTime-startTime);
        startTime = stopTime;
#endif
    }
}

void app_main()
{
    nvs_flash_init();

    leds.init();
    status.setLedController(&leds);
    status.setStatus(OTA_AND_BT_OK);
    xTaskCreatePinnedToCore(taskLoop, "Status_Task", 4096, NULL, 9, &statusTask, 0);
    wifi_init_sta();
    startOtaTask(1, 0);
    robotCtrl.Init("MARRAJO", robotConfig, 7);
    phyinputs.init();
    uSerial.Init();

 
#if 0
        //vTaskDelay(pdMS_TO_TICKS(0));
        /*motor1.Init(250, 125, 250, 1);
        motor2.Init(250, 125, 250);
        succion.Init(250, 125, 250);*/
        printf("Connect the ESC now!\n");
        printf("Wait for 2 beeps from the ESC and press a key\n");

        waitForChar();
        motor1.setPowerPercentage(0);
        motor2.setPowerPercentage(0);
        succion.setPowerPercentage(0);
        printf("Wait for 1,2,3 beeps \n");
        waitForChar();
#else
    motor1.Init(125, 125, 250, 1);
    motor2.Init(125, 125, 250);
    succion.Init(125, 125, 250);
    vTaskDelay(pdMS_TO_TICKS(5000));
#endif
    

    printf("Ready!\n");
    status.setStatus(MOTORS_OK);
    morro.init();

    /*uint16_t values[16] = {0};
    while (1)
    {
        for (uint8_t i = 0; i < ARRAY_NUM_SENSOR / 2; i++)
        {
            // updateAndReadSensorArrayRaw(2 * i, 2 * i + 1, tempValueE, tempValueO);
            sensors.configExpander(2 * i, 2 * i + 1);
            sensors.readSensors(values[2 * i], values[2 * i + 1]);
            // updateCalibration(2 * i, tempValueE);
            // updateCalibration(2 * i + 1, tempValueO);
        }
        printf("data: ");
        for (uint8_t i = 0; i < ARRAY_NUM_SENSOR; i++)
        {
            printf("%-5u ", values[i]);
        }
        printf("\n");
    }*/

    status.setStatus(CALIBRATING);
    uint32_t cnt = 0;
    while (morro.calibrate() == 0 && status.getStatus() != OTA_UPDATE)
    {
        cnt++;
        if(cnt == 1000)
        {
            uSerial.printf("Dato motor: %.2f\n", succion.getPowerPercentage());
            cnt = 0;
        }
        
    }
    if (status.getStatus() == OTA_UPDATE)
    {
        sensors.stopAdc();
        while (1)
            ;
    }
    status.setStatus(WAITING_START);
    while (phyinputs.getInput(0) == 1 && status.getStatus() != OTA_UPDATE)
    {
        morro.calibrate();
    }
    vTaskDelay(pdMS_TO_TICKS(300));
    while (phyinputs.getInput(0) == 0);
    status.setStatus(MOTORS_OK);
    vTaskDelay(pdMS_TO_TICKS(1000));
    while (phyinputs.getInput(0) == 1);
    status.setStatus(NOMINAL);
    morro.searchLinePosition();
    vTaskDelay(pdMS_TO_TICKS(3000));
    xTaskCreatePinnedToCore(taskMorro, "Morro_Task", 4096, NULL, 9, &morroTask, 1);
    uint8_t isWifiInit = 0;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}