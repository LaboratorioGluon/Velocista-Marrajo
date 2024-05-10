#ifndef __GPIOEXPANDER_H__
#define __GPIOEXPANDER_H__

#include <driver/gpio.h>
#include <driver/i2c.h>


struct gpioExpanderDef{
    i2c_port_t pI2cPort;
    uint8_t pAddr;
    gpio_num_t sdaPin;
    gpio_num_t sclPin;
};

class gpioExpander{
public:
    gpioExpander(struct gpioExpanderDef _gpioDef);

    void init();

    /**
     * Set all the outputs of the GPIO.
     * Bit N -> Output N
     * i.e.: 0b00001001 => Output 0 and 3, HIGH, otherwise LOW.
    */
    uint8_t setOutputs(uint8_t allValues);

    /**
     * Configure the mode of all the GPIOs.
     * Bit N set -> Gpio N to Input
     * i.e.: 0b00001001 => Gpios 0 and 3, Inputs, otherwise Output.
    */
    uint8_t configOutputs(uint8_t inOutGpios);


private:

    struct gpioExpanderDef gpioDef;
    uint8_t write_buf[2];
    static uint8_t gpioex_i2c_buf[I2C_LINK_RECOMMENDED_SIZE(3)];

};

#endif //__GPIOEXPANDER_H__