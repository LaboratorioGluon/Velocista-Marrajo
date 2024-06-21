#include "comms/BTCommands.h"
#include "BtRobotController.h"

#include <driver/gpio.h>
#include "HwDevices.h"

#include <common/pid.h>

extern float targetSpeed;
extern uint8_t stopOrder;

// Si es operacion READ, en el puntero a data  guardare los datos que quiera
// Si es operacion WRITE, en el puntero leere los datos.
uint32_t cbStop(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        BtRobotController::data_op_read(&stopOrder, sizeof(stopOrder));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(uint8_t))
        {
            stopOrder = 1;
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

extern float targetSpeed;
uint32_t cbVelocidad(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        BtRobotController::data_op_read(&targetSpeed, sizeof(targetSpeed));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(uint32_t))
        {
            targetSpeed = *(uint32_t*)data;
        }
    }

    return 0;
}

extern PID motorPid;
uint32_t cbKp(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        float kp = motorPid.getKp();
        BtRobotController::data_op_read(&kp, sizeof(kp));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(float))
        {
            motorPid.setKp(*(float*)data);
        }
    }

    return 0;
}

uint32_t cbKi(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        float ki = motorPid.getKi();
        BtRobotController::data_op_read(&ki, sizeof(ki));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(float))
        {
            motorPid.setKi(*(float*)data);
        }
    }

    return 0;
}

uint32_t cbKd(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        float kd = motorPid.getKd();
        BtRobotController::data_op_read(&kd, sizeof(kd));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(float))
        {
            motorPid.setKd(*(float*)data);
        }
    }

    return 0;
}


uint32_t cbSucc(void * data, uint32_t len, BtRobotOperationType operation)
{
    if(operation == BTROBOT_OP_READ)
    {
        //BtRobotController::data_op_read(&meanspeed, sizeof(meanspeed));
    }
    else if(operation == BTROBOT_OP_WRITE)
    {
        if(len == sizeof(uint32_t))
        {
            succion.setPowerPercentage(*(uint32_t*)data);
        }
    }

    return 0;
}


struct BtRobotConfiguration robotConfig[]={
    {"Stop"         , cbStop           , {BTROBOT_CONFIG_EVENT         , {}                            }},
    {"Start"        , readCallback1    , {BTROBOT_CONFIG_EVENT         , {}                            }},
    {"Velocidad"    , cbVelocidad      , {BTROBOT_CONFIG_INT_SLIDE     , {.intSlide{   0 ,  100,    1 }   }   }},
    {"Kp"           , cbKp             , {BTROBOT_CONFIG_FLOAT_SLIDE   , {.floatSlide{ 0.0f, 0.5f, 0.1f }   }   }},
    {"Ki"           , cbKi             , {BTROBOT_CONFIG_FLOAT_SLIDE   , {.floatSlide{ 0.0f, 1.0f, 0.1f }   }   }},
    {"Kd"           , cbKd             , {BTROBOT_CONFIG_FLOAT_SLIDE   , {.floatSlide{ 0.0f, 5.0f, 0.1f }   }   }},
    {"Succ"         , cbSucc           , {BTROBOT_CONFIG_INT_SLIDE     , {.intSlide{ 0, 100, 1 }            }   }},
};

/*
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
        {
        .paramName="Kp",
        .callback = cbVelocidad,
        .dataConfig= {
            .dataType = BTROBOT_CONFIG_FLOAT_SLIDE,
            .config{.floatSlide{ 0, 100, 1 }}
        }
    },
        {
        .paramName="Ki",
        .callback = cbVelocidad,
        .dataConfig= {
            .dataType = BTROBOT_CONFIG_INT_SLIDE,
            .config{.intSlide{ 0, 100, 1 }}
        }
    },
        {
        .paramName="Kd",
        .callback = cbVelocidad,
        .dataConfig= {
            .dataType = BTROBOT_CONFIG_INT_SLIDE,
            .config{.intSlide{ 0, 100, 1 }}
        }
    },

};*/


