#ifndef __COMMON_LOG_H__
#define __COMMON_LOG_H__

#include <stdint.h>
#include <comms/udp_serial.h>

struct logInfo{
    uint16_t index;
    int16_t linePos;
    int16_t pidOutput;
    int8_t motorR;
    int8_t motorL;
    uint16_t dt;
};


void resetLog();
void storeLoopInfo(logInfo newInfo);
void dumpDataToUdp(UdpSerial& udp);

#endif