#ifndef __STATES_H__
#define __STATES_H__

enum STATUS{
    OTA_AND_BT_OK = 0, // First step is to start OTA and BT
    MOTORS_OK,
    CALIBRATING,
    OTA_UPDATE,
    WAITING_START,
    LINE_LOST,
    NOMINAL,
    DUMPING_LOG,
    SECURITY_STOP
};


#endif //__STATES_H__