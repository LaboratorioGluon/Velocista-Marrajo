#include "comms/BTCommands.h"
#include "BtRobotController.h"

#include <driver/gpio.h>
#include "HwDevices.h"

// Si es operacion READ, en el puntero a data  guardare los datos que quiera
// Si es operacion WRITE, en el puntero leere los datos.
uint32_t cbStop(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        //BtRobotController::data_op_read(&gpioValue, sizeof(gpioValue));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(uint8_t))
        {
            /*memcpy(&gpioValue, data, len);
            gpio_set_level(GPIO_NUM_5, gpioValue);*/
            //motors.setDirection(gpioValue?MotorController::DIRECTION::FWD:MotorController::DIRECTION::STOP);
            motor1.setPowerPercentage(0);
            motor2.setPowerPercentage(0);
        }
    }

    return 0;
}

uint32_t readCallback1(void * data, uint32_t len, BtRobotOperationType operation)
{
    ESP_LOGE("CALLBACK1","Ejecutando callback 1!!!!");
    if(operation == BTROBOT_OP_READ)
    {
        //BtRobotController::data_op_read(&gpioValue, sizeof(gpioValue));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(uint8_t))
        {
            
        }
    }
    return 0;
}

uint32_t readCallback2(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        //BtRobotController::data_op_read(&totalMove, sizeof(totalMove));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        /*if(len == sizeof(uint32_t))
        {
            memcpy(&totalMove, data, len);
        }*/
    }

    return 0;
}

uint32_t readCallback3(void * data, uint32_t len, BtRobotOperationType operation)
{
    ESP_LOGE("CALLBACK3","Ejecutando callback 3!!!!");
    return 0;
}

uint32_t cbVelocidad(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        //BtRobotController::data_op_read(&totalMove, sizeof(totalMove));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(uint32_t))
        {
            motor1.setPowerPercentage(*(uint32_t*)data);
            motor2.setPowerPercentage(*(uint32_t*)data);
            //memcpy(&totalMove, data, len);
        }
    }

    return 0;
}

/*
struct BtRobotConfiguration robotConfig2[]={
    {"Toggle LED"   , updateGPIO    , {BTROBOT_CONFIG_LATCH         , {}                            }},
    {"Push"         , updateGPIO    , {BTROBOT_CONFIG_LATCH         , {}                            }},
    {"Velocidad"    , cbVelocidad   , {BTROBOT_CONFIG_FLOAT_SLIDE   , {.floatSlide{ 0, 3, 0.01 }}   }},
};
*/

struct BtRobotConfiguration robotConfig[]={
    {
        .paramName="Stop",
        .callback = cbStop,
        .dataConfig= {
            .dataType = BTROBOT_CONFIG_EVENT,
        }
    },
        {
        .paramName="Start",
        .callback = readCallback1,
        .dataConfig= {
            .dataType = BTROBOT_CONFIG_EVENT,
        }
    },
    {
        .paramName="Velocidad",
        .callback = cbVelocidad,
        .dataConfig= {
            .dataType = BTROBOT_CONFIG_INT_SLIDE,
            .config{.intSlide{ 0, 100, 1 }}
        }
    },

};


