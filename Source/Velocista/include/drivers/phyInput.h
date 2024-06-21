#ifndef __PHYINPUT_H__
#define __PHYINPUT_H__

#include <stdint.h>

#include <driver/gpio.h>


class phyInput{
public:
    phyInput();

    void init();

    void waitForInput(uint8_t buttonIndex);
    
    uint8_t getInput(uint8_t buttonIndex);

private:
    gpio_config_t gpioConfig;   

};

#endif //__PHYINPUT_H__