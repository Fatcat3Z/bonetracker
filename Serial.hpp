#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>

#define BAUDRATE 115200//B115200//设置条码扫描仪的串口通讯波特率为115200BPS
#define COM2 "/dev/ttyUSB0"

class MySerial {
public:
    MySerial();

    ~MySerial();

public:
    static int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop);

    static int open_port(int comport);

    static int nwrite(int serialfd, unsigned char *data, int datalength);

    static int nread(int fd, unsigned char *data, int datalength);
};

#endif