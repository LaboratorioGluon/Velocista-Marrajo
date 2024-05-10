#ifndef __COMMON_H__
#define __COMMON_H__

#include <esp_adc/adc_oneshot.h>

struct adcOneShotDef{
    adc_unit_t unit;
    adc_channel_t chan;
};

#endif //__COMMON_H__