#ifndef __CUSTOM_ADC_H__
#define __CUSTOM_ADC_H__

#include <esp_adc/adc_continuous.h>
#ifdef __cplusplus
extern "C" {
#endif
esp_err_t adc_continuous_pause(adc_continuous_handle_t handle);
esp_err_t adc_continuous_resume(adc_continuous_handle_t handle);

#ifdef __cplusplus
}
#endif
#endif //__CUSTOM_ADC_H__