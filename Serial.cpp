//
// Created by fatcat on 2/27/19.
//

#include "Serial.hpp"

MySerial::MySerial() {
}

MySerial::~MySerial() {

}

int MySerial::set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)  //串口设置
{
    struct termios newtio, oldtio;
    //tcgetattr是一个函数，用来获取终端参数，成功返回零；失败返回非零，发生失败接口将设置errno错误标识
    if (tcgetattr(fd, &oldtio) != 0) {
        perror("SetupSerial 1");
        return -1;
    }
    bzero(&newtio, sizeof(newtio));//置字节字符串前n个字节为零且包括‘\0’。
    newtio.c_cflag |= CLOCAL | CREAD;//忽略调制解调器线路状态//使用接收器
    newtio.c_cflag &= ~CSIZE;//字符长度，取值范围为CS5、CS6、CS7或CS8
    switch (nBits) {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size.\n");
            return -1;
    }

/*设置奇偶校验位*/
    switch (nEvent) {
        case 'O':
            newtio.c_cflag |= PARENB;//使用奇偶校验
            newtio.c_cflag |= PARODD;//对输入使用奇偶校验，对输出使用偶校验
            newtio.c_iflag |= (INPCK | ISTRIP);//INPCK允许输入奇偶校验//去除字符的第8个比特
            break;
        case 'E':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N':
            newtio.c_cflag &= ~PARENB;//使用奇偶校验
            newtio.c_oflag = ~ICANON;
            break;
        default:
            fprintf(stderr, "Unsupported parity.\n");
            return -1;
    }

    switch (nSpeed) {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        default:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }

    switch (nStop) //设置停止位
    {
        case 1:
            newtio.c_cflag &= ~CSTOPB;//设置两个停止位
            break;
        case 2:
            newtio.c_cflag |= CSTOPB;//设置两个停止位
            break;
        default:
            fprintf(stderr, "Unsupported stopbits.\n");
            return -1;
    }

    newtio.c_cc[VTIME] = 0;//c_cc[NCCS]：控制字符，用于保存终端驱动程序中的特殊字符，如输入结束符等。
    //VTIME 非规范模式读取时的超时时间
    newtio.c_cc[VMIN] = 0;//非规范模式读取时的最小字符数
    tcflush(fd, TCIFLUSH);//Unix终端I/O函数。作用：清空终端未完成的输入/输出请求及数据。
//tcsetattr函数用于设置终端参数。函数在成功的时候返回0，失败的时候返回-1，并设置errno的值
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
        perror("com set error");
        return -1;
    }
    return 0;
}

int MySerial::open_port(int comport)                 //通过参数，打开相应的串口
{
    int fd;
    if (comport == 1) {
        //O_RDWR读写模式 O_NOCTTY如果路径名指向终端设备,不要把这个设备用作控制终端 O_NDELAY表示不关心DCD信号所处的状态（端口的另一端是否激活或者停止）
        fd = open("/dev/pts/19", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
        if (-1 == fd) {
            perror("Can't Open Serial /dev/ttyUSB0 Port");
            return (-1);
        }
    } else if (comport == 2) {
        fd = open("/dev/pts/20", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
        if (-1 == fd) {
            perror("Can't Open Serial /dev/ttyUSB1 Port");
            return (-1);
        }
    }

//    if (fcntl(fd, F_SETFL, 0) < 0)
//        perror("fcntl failed!");
//    else
//        printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));

    if (isatty(STDIN_FILENO) == 0)
        perror("standard input is not a terminal device");
    else
        printf("isartty success!\n");
    printf("fd-open=%d\n", fd);
    return fd;
}

int MySerial::nwrite(int serialfd, unsigned char *data, int datalength)  //写串口信息
{
    int len = 0, total_len = 0;
    // for (total_len = 0 ; total_len < datalength;)
    // {
    len = 0;
    //len = write(serialfd, &data[total_len], datalength - total_len);
    for (int i = 0; i < datalength; i++) {
        len += write(serialfd, &data[i], 1);//如果顺利write()会返回实际写入的字节数。当有错误发生时则返回-1，错误代码存入errno中。
        //usleep(1000);
    }
//         if (len > 0)
//             total_len += len;
//    }
    return (len);
}

int MySerial::nread(int fd, unsigned char *data, int datalength)   //读取串口信息
{
    int readlen = 0;
    readlen = static_cast<int>(read(fd, data, datalength));
//    if (readlen> 0) {
//        char *q= static_cast<char *>(data);
//        printf("current condition is %d\n", int(*q));
//    }
    return readlen;
}
