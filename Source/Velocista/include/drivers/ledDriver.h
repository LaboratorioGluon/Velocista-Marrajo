#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__


extern "C"
{
    #include "led_strip_encoder.h"
}

#ifndef LEDDRIVER_NUM_OF_LEDS 
    #define LEDDRIVER_NUM_OF_LEDS 10U
#endif 


class ledDriver
{
public:
    ledDriver(gpio_num_t gpioNum, uint32_t numOfLeds);

    void init();

    void setColor(uint32_t ledIndex, uint8_t r, uint8_t g, uint8_t b);
    void update();


private:

    rmt_channel_handle_t led_chan;
    rmt_encoder_handle_t led_encoder;
    uint8_t ledGBRs[LEDDRIVER_NUM_OF_LEDS*3]; // times 3, because each led has G,B,R

    gpio_num_t gpioPin;
    uint32_t numberOfLeds;
};

#endif //__LED_DRIVER_H__