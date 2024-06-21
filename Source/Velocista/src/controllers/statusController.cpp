#include "controllers/statusController.h"
#include "string.h"
#include <esp_timer.h>

statusController &statusController::getInstance()
{
    static statusController instance;
    return instance;
}

statusController::statusController()
{
    memset(ledColors, 0, 6);
}

void statusController::setStatus(enum STATUS newStatus)
{
    currentStatus = newStatus;
    switch (newStatus)
    {
    case OTA_AND_BT_OK:
        configLed(1, 8, 0, 0, 500);
        break;
    case MOTORS_OK:
        configLed(1, 8, 0, 0, 0);
        break;
    case OTA_UPDATE:
        configLed(1, 8, 5, 0, 250);
        configLed(0, 8, 5, 0, 250);
        break;
    case CALIBRATING:
        configLed(1, 0, 8, 0, 100);
        configLed(0, 0, 8, 0, 100);
        break;
    case WAITING_START:
        configLed(1, 0, 8, 0, 0);
        configLed(0, 0, 8, 0, 0);
        break;
    case NOMINAL:
        configLed(1, 0, 0, 8, 0);
        configLed(0, 0, 0, 8, 0);
        break;
    case SECURITY_STOP:
        configLed(1, 8, 0, 0, 250);
        configLed(0, 8, 0, 0, 250);
        break;
    case DUMPING_LOG:
        configLed(1, 0, 0, 8, 250);
        configLed(0, 0, 0, 8, 250);
        break;
    default:
        break;
    }
}

void statusController::loop(uint32_t dtms)
{
    // Check led blinking
    for (uint8_t i = 0; i < 2; i++)
    {
        if (ledBlinkPeriodMs[i] > 0)
        {
            // If the period has expired.
            if ((esp_timer_get_time() - ledBlinkPeriodStart[i]) > ledBlinkPeriodMs[i])
            {
                ledBlinkPeriodStart[i] = esp_timer_get_time();

                if (ledBlinkStatus[i] == 0)
                {
                    ledControler->setColor(i, ledColors[i][0], ledColors[i][1], ledColors[i][2]);
                    ledBlinkStatus[i] = 1;
                }
                else
                {
                    ledControler->setColor(i, 0, 0, 0);
                    ledBlinkStatus[i] = 0;
                }
            }
        }
        else
        {
            ledControler->setColor(i, ledColors[i][0], ledColors[i][1], ledColors[i][2]);
            ledBlinkStatus[i] = 1;
        }
    }
    ledControler->update();
}

void statusController::enableLedBlink(uint32_t ledIndex, uint32_t periodMs)
{
    ledBlinkPeriodStart[ledIndex] = esp_timer_get_time();
    ledBlinkPeriodMs[ledIndex] = periodMs;
}

void statusController::disableLedBlink(uint32_t ledIndex)
{
    ledBlinkPeriodMs[ledIndex] = 0;
}

void statusController::configLed(uint32_t ledIndex, uint8_t r, uint8_t g, uint8_t b, uint32_t periodMs)
{
    ledColors[ledIndex][0] = r;
    ledColors[ledIndex][1] = g;
    ledColors[ledIndex][2] = b;
    if (periodMs > 0)
    {
        enableLedBlink(ledIndex, periodMs);
    }
    else
    {
        disableLedBlink(ledIndex);
    }
}