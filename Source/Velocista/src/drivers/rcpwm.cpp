#include "drivers/rcpwm.h"

#include "esp_log.h"
static const char* TAG = "RcPWM";

RcPwm::RcPwm(ledc_channel_t _channel,gpio_num_t _pinEsc, uint8_t _isInverted)
    :isInverted(_isInverted),isInitialized(false), pinEsc(_pinEsc), channelLedC(_channel)
{
    rateOfChange = 15; // 180º = 100 -> 1000 ms
}

void RcPwm::setMicrosecondsUp(uint32_t microseconds)
{
    uint32_t duty = microseconds * ((1<<13) - 1)/250;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channelLedC, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channelLedC);
}

void RcPwm::setRawDuty(uint32_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channelLedC, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channelLedC);
}
#include <esp_timer.h>

/* Configure the PWM to 50Hz */
void RcPwm::Init(uint32_t initMs, uint32_t _minPwm , uint32_t _maxPwm, uint8_t initTimer)
{
    
    if(initTimer)
    {
            
        ledc_timer_config_t ledc_timer;

        ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_timer.timer_num = LEDC_TIMER_0;
        ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;
        ledc_timer.freq_hz = 4000;
        ledc_timer.clk_cfg = LEDC_AUTO_CLK;
        ledc_timer.deconfigure = false;

        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    }

    ledc_channel_config_t ledc_channel;
    
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.channel = channelLedC;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;
    ledc_channel.gpio_num = pinEsc;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;
    ledc_channel.flags.output_invert = isInverted;

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));


    isInitialized = true;

    eqSlope = (_maxPwm - _minPwm)/100.0f;
    eqOffset = _minPwm;

    setMicrosecondsUp(initMs);
}

void RcPwm::setPowerPercentage(uint32_t power)
{
    //ESP_LOGE(TAG, "Setting power to: %lu", power);
    current = power*1.0f;
    uint32_t microseconds = eqSlope*power + eqOffset;
    setMicrosecondsUp(microseconds);
}


void RcPwm::setTargetPercentage(uint32_t atarget)
{
    target = atarget;
}

void RcPwm::update(uint32_t deltaTms)
{   
    float deltaMax = (rateOfChange*deltaTms)/1000.0f;
    float delta = (target-current);
    //ESP_LOGE(TAG, "deltaMax: %.3f, delta: %.3f, deltaMs: %lu", deltaMax, delta, deltaTms);
    float prevCurr = current;
    if(abs(delta) > deltaMax){  
        if(delta > 0.0f)
        {
            setPowerPercentage( current + deltaMax);
            current = prevCurr + deltaMax;
        }
        else
        {
            
            setPowerPercentage( current - deltaMax);
            current = prevCurr - deltaMax;
        }
    }else{
        setPowerPercentage( current + delta);
        current = prevCurr + delta;
    }

}