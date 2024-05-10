#include "controllers/morroController.h"

morroController::morroController(sensorArray &pSensors) : sensors(pSensors)
{
    aCurrentSensors[0] = 7;
    aCurrentSensors[1] = 8;

    isCalibrationCompleted = 0;
    for (uint8_t i = 0; i < ARRAY_NUM_SENSOR; i++)
    {
        calLowValue[i] = 1024;
        calHighValue[i] = 0;
        calDistance[i] = calHighValue[i] - calLowValue[i];
    }
}

void morroController::init()
{
    sensors.init();
}

uint8_t morroController::calibrate()
{
    uint16_t tempValueE, tempValueO;
    for (uint8_t i = 0; i < ARRAY_NUM_SENSOR / 2; i++)
    {
        updateAndReadSensorArrayRaw(2 * i, 2 * i + 1, tempValueE, tempValueO);
        updateCalibration(2 * i, tempValueE);
        updateCalibration(2 * i + 1, tempValueO);
    }

    int16_t minDist = 9999;
    isCalibrationCompleted = 1;
    for (uint8_t i = 0; i < ARRAY_NUM_SENSOR; i++)
    {
        if (calDistance[i] < minDist)
        {
            minDist = calDistance[i];
        }
        printf("%d ", calDistance[i]);
    }
    printf("\n");
    if (minDist < 400)
    {
        isCalibrationCompleted = 0;
    }
    return isCalibrationCompleted;
}

uint8_t morroController::updateCalibration(uint8_t sensorIndex, uint16_t newValue)
{
    if (newValue < calLowValue[sensorIndex])
    {
        calLowValue[sensorIndex] = newValue;
        calDistance[sensorIndex] = calHighValue[sensorIndex] - calLowValue[sensorIndex];
    }
    if (newValue > calHighValue[sensorIndex])
    {
        calHighValue[sensorIndex] = newValue;
        calDistance[sensorIndex] = calHighValue[sensorIndex] - calLowValue[sensorIndex];
    }
    return 0;
}

int32_t morroController::getLinePosition()
{
    updateSensorsToRead();

    updateAndReadSensorArray(sensorsToRead[1], sensorsToRead[2], sensorsToReadData[1], sensorsToReadData[2]);
    updateAndReadSensorArray(sensorsToRead[0], sensorsToRead[3], sensorsToReadData[0], sensorsToReadData[3]);

    /*if (sensorsToReadData[3] > 600 && sensorsToReadData[1] < 600)
    {
        aCurrentSensors[0]++;
        aCurrentSensors[1]++;
        // printf("%lu %lu\n", aCurrentSensors[0], aCurrentSensors[1]);
    }
    else if (sensorsToReadData[0] > 600 && sensorsToReadData[2] < 600)
    {
        aCurrentSensors[0]--;
        aCurrentSensors[1]--;
        // printf("%lu %lu\n", aCurrentSensors[0], aCurrentSensors[1]);
    }*/

    int32_t acum = 0;
    int32_t weightened = 0;
    float score[4] = {0.2f, 1, 1, 0.2f};
    for (uint8_t i = 0; i < 4; i++)
    {
        weightened += ((float)sensorsToReadData[i])*score[i] * ((float)sensorsToRead[i] - ((16.0f - 1.0f) /2.0f)) * 100;
        acum += sensorsToReadData[i];
    }

    printf("%-5d %-5d %-5d %-5d",
           sensorsToReadData[0],
           sensorsToReadData[1],
           sensorsToReadData[2],
           sensorsToReadData[3]);

    return (int32_t)weightened / acum;
}

void morroController::updateSensorsToRead()
{
    sensorsToRead[1] = aCurrentSensors[0];
    sensorsToRead[2] = aCurrentSensors[1];

    sensorsToRead[0] = sensorsToRead[1] - 1;
    sensorsToRead[3] = sensorsToRead[2] + 1;
}



void morroController::updateAndReadSensorArrayRaw(uint32_t v1, uint32_t v2, uint16_t &datav1, uint16_t &datav2)
{
    if (v1 % 2 == 0)
    {
        sensors.configExpander(v1, v2);
        sensors.readSensors(datav1, datav2);
    }
    else
    {
        sensors.configExpander(v2, v1);
        sensors.readSensors(datav2, datav1);
    }

}

void morroController::updateAndReadSensorArray(uint32_t v1, uint32_t v2, uint16_t &datav1, uint16_t &datav2)
{
    
    updateAndReadSensorArrayRaw(v1, v2 , datav1, datav2);
    // Normalizamos los datos de 0 a 100
    /*long map(long x, long in_min, long in_max, long out_min, long out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }*/

    datav1 = (datav1 - calLowValue[v1]) * 100 / calDistance[v1];
    datav2 = (datav2 - calLowValue[v2]) * 100 / calDistance[v2];
}