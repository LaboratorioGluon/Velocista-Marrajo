#ifndef __SENSOR_ARRAY_H__
#define __SENSOR_ARRAY_H__

#include <esp_adc/adc_oneshot.h>
#include <esp_adc/adc_continuous.h>
#include <driver/i2c.h>

#include "gpioExpander.h"
#include "drivers/custom_adc.h"


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
    void initCont();

    void readSensors(uint16_t &evenValue, uint16_t &oddValue);

    uint8_t configExpander( uint8_t  sensorE, uint8_t sensorO);

    void stopAdc(){ adc_continuous_resume(handle); adc_continuous_stop(handle); adc_continuous_deinit(handle);}

private:

    gpioExpander gpioEx;
    
    adc_oneshot_unit_handle_t hAdc;
    adcOneShotDef configEven, configOdd;

    gpio_num_t pinMosfet;

    adc_continuous_handle_t handle;
    adc_continuous_config_t dig_cfg;
    adc_continuous_handle_cfg_t adc_config;
    adc_continuous_evt_cbs_t cbs;

};

#endif //__SENSOR_ARRAY_H__