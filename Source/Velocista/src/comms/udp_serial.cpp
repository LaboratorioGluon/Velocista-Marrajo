#include "comms/udp_serial.h"

#include <esp_log.h>

#include <stdio.h>
#include <stdarg.h>

static const char TAG[] = "SOCKET";

UdpSerial::UdpSerial(char *ip, uint16_t port)
{
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
}

void UdpSerial::Init()
{
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    }

}

void UdpSerial::printf(const char *format, ...)
{
    va_list argp;
    va_start(argp, format);
    vsprintf(writeBuffer, format, argp);
    int err = sendto(sock, writeBuffer, strlen(writeBuffer), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
    va_end(argp);
}
