#ifndef __STATUS_CONTROLLER_H__
#define __STATUS_CONTROLLER_H__

#include "drivers/ledDriver.h"

enum STATUS{
    OTA_AND_BT_OK = 0, // First step is to start OTA and BT
    MOTORS_OK,
    CALIBRATING,
    OTA_UPDATE,
    WAITING_START,
    LINE_LOST,
    NOMINAL,
    SECURITY_STOP
};

class statusController{
public:
    static statusController& getInstance();

    void setLedController(ledDriver *leds){ ledControler = leds;}

    void setStatus(enum STATUS newStatus);

    void loop(uint32_t dtms);

private:

    statusController();

    enum STATUS currentStatus;

    ledDriver* ledControler = nullptr;
    uint32_t ledBlinkPeriodMs[2] = {0};
    uint64_t ledBlinkPeriodStart[2] = {0};
    uint8_t  ledBlinkStatus[2] = {0};
    uint8_t  ledColors[2][3] = {0};


    inline void configLed(uint32_t ledIndex, uint8_t r,  uint8_t g,  uint8_t b, uint32_t periodMs);

    inline void enableLedBlink(uint32_t ledIndex, uint32_t periodMs);
    inline void disableLedBlink(uint32_t ledIndex);
};

#endif //__STATUS_CONTROLLER_H__