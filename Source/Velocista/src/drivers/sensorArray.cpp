#include "drivers/sensorArray.h"
#include <esp_adc/adc_continuous.h>
#include <freertos/FreeRTOS.h>

#include <hal/adc_hal.h>

#include "drivers/custom_adc.h"

#include <string.h>

constexpr uint32_t NUM_MEDIDAS_ADC = 5;

uint8_t dataReady = 0;

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    dataReady = 1;

    return (mustYield == pdTRUE);
}

sensorArray::sensorArray(struct gpioExpanderDef gpioDef,
                         struct adcOneShotDef defEven, 
                         struct adcOneShotDef defOdd,
                         gpio_num_t pPinMosfet) : 
                         gpioEx(gpioDef), configEven(defEven), configOdd(defOdd), pinMosfet(pPinMosfet)
{
    handle = NULL;
}

void sensorArray::initCont()
{

    adc_config = {
        .max_store_buf_size = 10*SOC_ADC_DIGI_DATA_BYTES_PER_CONV,
        .conv_frame_size = 10*SOC_ADC_DIGI_DATA_BYTES_PER_CONV,
        .flags = {
            .flush_pool =0 
            }
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    dig_cfg = {
        .pattern_num = 2,
        .sample_freq_hz = 200 * 1000,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1
    };

    adc_digi_pattern_config_t adc_pattern[2];
    adc_pattern[0].atten = ADC_ATTEN_DB_11;
    adc_pattern[0].bit_width = ADC_BITWIDTH_10;
    adc_pattern[0].channel = configEven.chan;
    adc_pattern[0].unit = ADC_UNIT_1;

    adc_pattern[1].atten = ADC_ATTEN_DB_11;
    adc_pattern[1].bit_width = ADC_BITWIDTH_10;
    adc_pattern[1].channel = configOdd.chan;
    adc_pattern[1].unit = ADC_UNIT_1;

    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    adc_continuous_start(handle);
    adc_continuous_pause(handle);

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

#include <esp_timer.h>
void sensorArray::readSensors(uint16_t &evenValue, uint16_t &oddValue)
{
    uint32_t evenMean = 0;
    uint32_t evenMeanCounter = 0;
    uint32_t oddMean = 0;
    uint32_t oddMeanCounter = 0;
    
    uint8_t result[20*SOC_ADC_DIGI_DATA_BYTES_PER_CONV];
    uint32_t resultLen;
    uint64_t startTime = esp_timer_get_time();

    adc_continuous_flush_pool(handle);
    adc_continuous_resume(handle);
    adc_continuous_read(handle, result, 20*SOC_ADC_DIGI_DATA_BYTES_PER_CONV, &resultLen, 3000);
    adc_continuous_pause(handle);

    for (int i = 0; i < resultLen; i += SOC_ADC_DIGI_RESULT_BYTES) {
        adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
        //printf("Data: %d %d\n", p->type1.channel, p->type1.data);
        if (p->type1.channel == configEven.chan)
        {
            evenMean += p->type1.data;
            evenMeanCounter++;
        }
        else if(p->type1.channel == configOdd.chan)
        {
            oddMean += p->type1.data;
            oddMeanCounter++;
        }
        else
        {
        }
    }   
    //printf("ADC time: %llu\n", esp_timer_get_time()-startTime);  
    
    if(evenMeanCounter != 0 && oddMeanCounter != 0)
    {
        evenValue = (uint16_t)evenMean/evenMeanCounter;
        oddValue = (uint16_t)oddMean/oddMeanCounter;
    }

    //printf("[Array] ReadSensor: %d %d\n", evenValue, oddValue);
}
