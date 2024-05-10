#ifndef __HW_DEVICES_H__
#define __HW_DEVICES_H__

#include <driver/i2c.h>
#include "drivers/rcpwm.h"

#include "drivers/sensorArray.h"
#include "drivers/ledDriver.h"
#include "drivers/phyInput.h"
#include "controllers/morroController.h"
#include "controllers/statusController.h"
#include <BtRobotController.h>

extern RcPwm motor2;
extern RcPwm motor1;

extern uint32_t motorSpeed;
extern uint32_t startStop;

extern struct gpioExpanderDef gpioDef;
extern struct adcOneShotDef adcE;
extern struct adcOneShotDef adcO;

extern sensorArray sensors;
extern BtRobotController& robotCtrl;
extern morroController morro;
extern statusController status;
extern ledDriver leds;

extern phyInput phyinputs;

#endif //__HW_DEVICES_H__