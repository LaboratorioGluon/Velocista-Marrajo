#include "controllers/morroController.h"


// @TODO: delete me
#include "comms/udp_serial.h"
extern UdpSerial uSerial;

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
    //sensors.init();
    sensors.initCont();
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
        //uSerial.printf("%d ", calDistance[i]);
    }
    printf("\n");
    /*uSerial.printf("==Cal Data");
    for ( uint8_t i = 0; i < ARRAY_NUM_SENSOR; i++)
    {
        uSerial.printf("Cal[%u]: %u %u", i, calLowValue[i], calHighValue[i]);
    }
    uSerial.printf("==Cal Data End");*/
    
    if (minDist < 300)
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

MORRO_RET morroController::getLinePosition(int32_t& linePos)
{
    updateSensorsToRead();
    MORRO_RET lineStatus = MORRO_LINE_LOST;

    updateAndReadSensorArray(sensorsToRead[1], sensorsToRead[2], sensorsToReadData[1], sensorsToReadData[2]);
    updateAndReadSensorArray(sensorsToRead[0], sensorsToRead[3], sensorsToReadData[0], sensorsToReadData[3]);

    //printf("getPosition time: %llu\n", esp_timer_get_time()- startTime);

    if ( sensorsToReadData[0] < 50 &&
         sensorsToReadData[1] < 50 &&
         sensorsToReadData[2] < 50 &&
         sensorsToReadData[3] < 50)
    {
        uSerial.printf("========== Line lost!\n");
        // Give 5 oportunities to find the lane, otherwise give up.
        for( int i = 0; i < 5 && lineStatus == MORRO_LINE_LOST; i++)
        {
            lineStatus = searchLinePosition();
        }
        if (lineStatus == MORRO_LINE_LOST)
        {
            return lineStatus;
        }
    }


    if (sensorsToReadData[3] > 50 && sensorsToReadData[1] < 50 && aCurrentSensors[1] < 14)
    {
        aCurrentSensors[0]++;
        aCurrentSensors[1]++;
    }
    else if (sensorsToReadData[0] > 50 && sensorsToReadData[2] < 50 && aCurrentSensors[0] > 1)
    {
        aCurrentSensors[0]--;
        aCurrentSensors[1]--;
    }

    int32_t acum = 0;
    int32_t weightened = 0;
    float score[4] = {1.0f, 1, 1, 1.0f};
    for (uint8_t i = 0; i < 4; i++)
    {
        weightened += ((float)sensorsToReadData[i])*score[i] * ((float)sensorsToRead[i] - ((16.0f - 1.0f) /2.0f)) * 100;
        acum += sensorsToReadData[i];
    }
    if (acum == 0)
    {
        acum = 1;
    }

    linePos = (int32_t)weightened / acum;
    return MORRO_LINE_OK;
}


MORRO_RET morroController::searchLinePosition()
{
    uint16_t tempValueE, tempValueO;

    for (uint8_t i = 0; i < ARRAY_NUM_SENSOR / 2; i++)
    {
        updateAndReadSensorArray(2 * i, 2 * i + 1, tempValueE, tempValueO);
        if (tempValueE > 50 && tempValueO > 50)
        {
            aCurrentSensors[0] = 2 * i;
            aCurrentSensors[1] = 2 * i + 1;
            return MORRO_LINE_OK;
        }
        else if (tempValueE > 50)
        {
            aCurrentSensors[0] = 2 * i - 1;
            aCurrentSensors[1] = 2 * i;
            return MORRO_LINE_OK;
        }
        else if (tempValueO > 50)
        {
            aCurrentSensors[0] = 2 * i + 1;
            aCurrentSensors[1] = 2 * i + 2;
            return MORRO_LINE_OK;
        }
    }

    return MORRO_LINE_LOST;
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

    if ( datav1 < calLowValue[v1])
    {
        datav1 = 0;
    }else
    {
        datav1 = (datav1 - calLowValue[v1]) * 100 / calDistance[v1];
    }
    if ( datav2 < calLowValue[v2])
    {
        datav2 = 0;
    }else
    {
        datav2 = (datav2 - calLowValue[v2]) * 100 / calDistance[v2];
    }

}