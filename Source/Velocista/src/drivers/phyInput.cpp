
#include "drivers/phyInput.h"


phyInput::phyInput()
{

}

void phyInput::init()
{
    
    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.intr_type = GPIO_INTR_DISABLE;
    gpioConfig.pin_bit_mask = (1<<GPIO_NUM_2) | (1<<GPIO_NUM_5);
    gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    
    gpio_config(&gpioConfig);
}

void phyInput::waitForInput(uint8_t buttonIndex)
{

}
   
uint8_t phyInput::getInput(uint8_t buttonIndex)
{
    if (buttonIndex == 0)
    {
        return gpio_get_level(GPIO_NUM_2);
    }
    else
    {
        return gpio_get_level(GPIO_NUM_5);
    }

}
