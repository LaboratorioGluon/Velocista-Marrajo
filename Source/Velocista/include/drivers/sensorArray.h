#ifndef __SENSOR_ARRAY_H__
#define __SENSOR_ARRAY_H__

#include <esp_adc/adc_oneshot.h>
#include <driver/i2c.h>

#include "gpioExpander.h"


struct adcOneShotDef{
    adc_unit_t unit;
    adc_channel_t chan;
};

constexpr uint16_t ARRAY_NUM_SENSOR = 16;

/**
 * @brief 
 * 
 * The pins go from 0 to 17. Being 0 the right most led.
 */
class sensorArray{
public:
    sensorArray(struct gpioExpanderDef gpioDef, 
                struct adcOneShotDef defEven, 
                struct adcOneShotDef defOdd, gpio_num_t pPinMosfet);


    void init();

    void readSensors(uint16_t &evenValue, uint16_t &oddValue);

    uint8_t configExpander( uint8_t  sensorE, uint8_t sensorO);

private:

    gpioExpander gpioEx;
    
    adc_oneshot_unit_handle_t hAdc;
    adcOneShotDef configEven, configOdd;

    gpio_num_t pinMosfet;

};

#endif //__SENSOR_ARRAY_H__