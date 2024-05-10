#ifndef __MORRO_CONTROLLER_H__
#define __MORRO_CONTROLLER_H__

#include "drivers/sensorArray.h"

class morroController{
public:
    morroController(sensorArray &pSensors);

    void init();

    uint8_t calibrate();

    int32_t getLinePosition();

private:

    sensorArray &sensors;

    // Last known line position.
    uint32_t aCurrentSensors[2];
    
    // Sensors to read 
    uint32_t sensorsToRead[4];
    uint16_t sensorsToReadData[4];

    // Calibration data
    uint8_t isCalibrationCompleted;
    uint16_t calLowValue[ARRAY_NUM_SENSOR];
    uint16_t calHighValue[ARRAY_NUM_SENSOR];
    int16_t calDistance[ARRAY_NUM_SENSOR];

    uint8_t updateCalibration(uint8_t sensorIndex, uint16_t newValue);
    
    void updateSensorsToRead();

    void updateAndReadSensorArrayRaw(uint32_t v1, uint32_t v2, uint16_t &datav1, uint16_t &datav2);
    void updateAndReadSensorArray(uint32_t v1, uint32_t v2, uint16_t &datav1, uint16_t &datav2);
    
};



#endif //__MORRO_CONTROLLER_H__