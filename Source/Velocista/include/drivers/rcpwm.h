#ifndef __RCPWM_H__
#define __RCPWM_H__

#include <driver/gpio.h>
#include <driver/ledc.h>

/**
 * @brief This class controls the ESC which controls the Brushless motor.
 * 
 */
class RcPwm{
public:
    RcPwm(ledc_channel_t _channel, gpio_num_t _pinEsc, uint8_t _isInverted = false);

    void Init(uint32_t initMs = 1000, uint32_t _minPwm = 1000, uint32_t _maxPwm  = 2000, uint8_t initTimer = 0);

    void setPowerPercentage(uint32_t power);
    void setTargetPercentage(uint32_t atarget);
    void update(uint32_t deltaTms);

    void setRate(uint32_t newRate){ rateOfChange = 300; }

    float getPowerPercentage(){ return current ; }

    void setRawDuty(uint32_t duty);
private:

    void setMicrosecondsUp(uint32_t miliseconds);

    uint8_t isInverted;

    uint8_t isInitialized;
    gpio_num_t pinEsc;
    ledc_channel_t channelLedC;

    uint32_t target;
    float current;

    uint32_t rateOfChange;

    float eqSlope, eqOffset;

};


#endif //__RCPWM_H__