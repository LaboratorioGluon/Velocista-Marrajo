#ifndef __UDP_SERIAL_H__
#define __UDP_SERIAL_H__
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/inet.h"
#include <lwip/netdb.h>
#include <stdint.h>

class UdpSerial
{
public:
    UdpSerial(char * ip, uint16_t port);

    void Init();

    void printf(const char* format, ...);
private:

    int sock;
    struct sockaddr_in dest_addr;

    char writeBuffer[200];

};


#endif //__UDP_SERIAL_H__