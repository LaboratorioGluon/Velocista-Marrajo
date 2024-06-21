#include <common/log.h>

constexpr uint32_t MAX_INFO_DATA = 6000;

static logInfo InfoData[MAX_INFO_DATA];
static uint32_t lastInfoData = 0;
static uint32_t logIndex = 0;


void resetLog()
{
    logIndex = 0;
    lastInfoData = 0;
}

void storeLoopInfo(logInfo newInfo)
{
    InfoData[lastInfoData] = newInfo;
    InfoData[lastInfoData].index = logIndex++;
    lastInfoData++;
    if (lastInfoData == MAX_INFO_DATA)
    {
        lastInfoData = 0;
    }
}

void dumpDataToUdp(UdpSerial& udp)
{
    uint32_t limit = (logIndex > MAX_INFO_DATA) ? MAX_INFO_DATA : logIndex;
    udp.printf("Index | linePos | pidOutpu | motorR | motorL | dt \n");
    for (uint32_t i = 0; i < limit; i++)
    {
        udp.printf("%lu %ld %ld %lu %lu %lu \n", InfoData[i].index,
                   InfoData[i].linePos,
                   InfoData[i].pidOutput,
                   InfoData[i].motorR,
                   InfoData[i].motorL,
                   InfoData[i].dt);
        vTaskDelay(1);
    }
}
