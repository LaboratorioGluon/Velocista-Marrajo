#include "drivers/sensorArray.h"

sensorArray::sensorArray(struct gpioExpanderDef gpioDef,
                         struct adcOneShotDef defEven, 
                         struct adcOneShotDef defOdd,
                         gpio_num_t pPinMosfet) : 
                         gpioEx(gpioDef), configEven(defEven), configOdd(defOdd), pinMosfet(pPinMosfet)
{
}

void sensorArray::init()
{
    adc_oneshot_unit_init_cfg_t init_config = {};
    init_config.clk_src = ADC_RTC_CLK_SRC_DEFAULT;
    init_config.ulp_mode = ADC_ULP_MODE_DISABLE;
    init_config.unit_id = ADC_UNIT_1;
    adc_oneshot_new_unit(&init_config, &hAdc);

    adc_oneshot_chan_cfg_t chanConfig = {};
    chanConfig.atten = ADC_ATTEN_DB_11;
    chanConfig.bitwidth = ADC_BITWIDTH_10;
    adc_oneshot_config_channel(hAdc, configEven.chan, &chanConfig);
    adc_oneshot_config_channel(hAdc, configOdd.chan, &chanConfig);

    gpioEx.init();
    gpioEx.configOutputs(0);

    gpio_config_t mosfetPins;
    mosfetPins.intr_type = GPIO_INTR_DISABLE;
    mosfetPins.mode = GPIO_MODE_OUTPUT;
    mosfetPins.pin_bit_mask = (1ULL << pinMosfet);
    mosfetPins.pull_down_en = GPIO_PULLDOWN_ENABLE;
    mosfetPins.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&mosfetPins);
    gpio_set_level(pinMosfet, 1);
}

uint8_t sensorArray::configExpander( uint8_t sensorE, uint8_t sensorO)
{
    uint8_t lsb = (sensorE/2) & 0b111;
    uint8_t msb = (((sensorO-1)/2) << 3U) & 0b111000;
    //printf("[Array] Setting %lu\n", (uint32_t)(msb | lsb));
    return gpioEx.setOutputs(msb | lsb);
}

void sensorArray::readSensors(uint16_t &evenValue, uint16_t &oddValue)
{
    int tempEven, tempOdd;
    //adc_oneshot_read(hAdc, configEven.chan, &(int&)evenValue);
    //adc_oneshot_read(hAdc, configOdd.chan, &(int&)oddValue);
    adc_oneshot_read(hAdc, configEven.chan, &tempEven);
    adc_oneshot_read(hAdc, configOdd.chan, &tempOdd);
    evenValue = (uint16_t)tempEven;
    oddValue = (uint16_t)tempOdd;
    //printf("[Array] ReadSensor: %d %d\n", evenValue, oddValue);
}