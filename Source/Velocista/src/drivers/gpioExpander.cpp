#include "drivers/gpioExpander.h"
#include <driver/i2c.h>
#include <esp_log.h>
#include <esp_timer.h>

uint8_t gpioExpander::gpioex_i2c_buf[I2C_LINK_RECOMMENDED_SIZE(3)] = {0};

gpioExpander::gpioExpander(struct gpioExpanderDef _gpioDef):gpioDef(_gpioDef)
{

}

void gpioExpander::init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = gpioDef.sdaPin, //,GPIO_NUM_26,
        .scl_io_num = gpioDef.sclPin, //GPIO_NUM_25,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master{
            .clk_speed = 400000,
        },
        .clk_flags = 0        
    };

    i2c_param_config(gpioDef.pI2cPort, &conf);
    ESP_ERROR_CHECK(i2c_driver_install(gpioDef.pI2cPort, conf.mode, 0, 0, 0));
}

uint8_t gpioExpander::setOutputs(uint8_t allValues)
{
    uint8_t ret;
    write_buf[0] = (gpioDef.pAddr << 1u) | I2C_MASTER_WRITE;
    write_buf[1] = 0b00000001;
    write_buf[2] = allValues;

    /*ret = i2c_master_write_to_device(gpioDef.pI2cPort, gpioDef.pAddr, 
        write_buf, sizeof(write_buf), 
        1000 / portTICK_PERIOD_MS 
    );*/

    i2c_cmd_handle_t cmd = i2c_cmd_link_create_static(gpioex_i2c_buf, sizeof(gpioex_i2c_buf));
    i2c_master_start(cmd);
    i2c_master_write( cmd, write_buf, 3, 1);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(gpioDef.pI2cPort, cmd, 0xFFFFF);
    i2c_cmd_link_delete_static(cmd);
    return ret;
}

uint8_t gpioExpander::configOutputs(uint8_t inOutGpios)
{
    uint8_t ret;
    uint8_t write_buf[2] = {0b00000011, inOutGpios};

    ret = i2c_master_write_to_device(gpioDef.pI2cPort, gpioDef.pAddr, 
        write_buf, sizeof(write_buf), 
        1000 / portTICK_PERIOD_MS
    );


    /*i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write( cmd, {(uint8_t)((gpioDef.pAddr << 1u) | I2C_MASTER_WRITE), 0b00000011, inOutGpios}, 3, 1);*/

    return ret;
}
