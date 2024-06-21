#include "HwDevices.h"

RcPwm motor1 (LEDC_CHANNEL_0,GPIO_NUM_19);
RcPwm motor2 (LEDC_CHANNEL_1,GPIO_NUM_17);
RcPwm succion(LEDC_CHANNEL_2,GPIO_NUM_16);

uint32_t motorSpeed = 0;
uint32_t startStop = 0;

struct gpioExpanderDef gpioDef = 
{
    .pI2cPort = I2C_NUM_0,
    .pAddr = 0x18,
    .sdaPin = GPIO_NUM_26,
    .sclPin = GPIO_NUM_25,
};

struct adcOneShotDef adcE{
    .unit = ADC_UNIT_1,
    .chan = ADC_CHANNEL_4
};

struct adcOneShotDef adcO{
    .unit = ADC_UNIT_1,
    .chan = ADC_CHANNEL_5
};

sensorArray sensors(gpioDef, adcE, adcO, GPIO_NUM_27);
BtRobotController& robotCtrl = BtRobotController::getBtRobotController();
morroController morro(sensors);
statusController status = statusController::getInstance();
ledDriver leds(GPIO_NUM_21, 2U);
phyInput phyinputs;