#include <thread>
#include <sys/time.h>
#include <csignal>
#include <string>
#include <unistd.h>
#include <termio.h>
#include "bonetaker.h"
#include "Threadinfo.h"
#include "BoneInfo.h"
#include "Serial.hpp"
#include "info.h"


using namespace std;
using namespace cv;

union Out wdata{};

#define calibration 0x1;

MySerial ms = MySerial();
int fd;

void printMes(int signo){                               //向外写数据
//    unsigned char wdata[4] = {0,0,0,0};
//    int readlen = ms.nread(fd, wdata, sizeof(wdata));
//    printf("readlen = %d\n", readlen);
//    for(int i = 0;i<sizeof(wdata);i++)
//        printf("%d\n", wdata[i]);
    //printf("Get a SIGALRM, signal NO:%d\n", signo);
    //cout<<"serial"<<endl;
    assignSum(&wdata);
    int a = ms.nwrite(fd, wdata.data, sizeof(wdata.data));

    cout<<"distance is"<<wdata.meta.mode3.deflection<<endl;
    //cout<<"size of outdata"<< sizeof(wdata.data)<<endl;
    wdata = {};
}

int main(){
    fd = ms.open_port(1);
    bool serialOpen = true;
    ms.set_opt(fd,BAUDRATE,8,'N',1);

    struct itimerval tick;
    signal(SIGALRM,printMes);
    memset(&tick, 0, sizeof(tick));
    tick.it_value.tv_sec = 0; //秒
    tick.it_value.tv_usec = 20000;//initial value 20000us
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 20000;//restart value 20000us
    if(setitimer(ITIMER_REAL,&tick,NULL) < 0)
        printf("error");

    BoneInfo boneInfo;
    Info info;
    Taker taker;

    bool videoused = true;

    while(true){
//        if (access("/dev/pts/19", R_OK | W_OK) == -1 || fd < 0) {
//                if (serialOpen) {
//                    close(fd);
//                    serialOpen = false;
//                }
//                continue;
//            } else if (!serialOpen) {
//                fd = ms.open_port(1);
//                serialOpen = true;
//            }
//        unsigned char rdata;
//        int n = ms.nread(fd, &rdata, 1);
//        //sometime read nothing
//        if (n <= 0)
//            continue;
        //cout << int(rdata) << endl;
        //cout << info.result.data << " length:" << sizeof(info.result.data) << endl;
        //if (info.push(rdata) <= 0)continue;

        bool tVideo1 = (access("/dev/video1", R_OK) != -1);

//        if(tVideo1){
            if(videoused){
                videoused = false;
                boneInfo.init();
                thread thread1(taker,ref(boneInfo));
                thread1.detach();
            }
            //if(info.result.meta.dataArea[2] & (1 << 2) != 0){
                double res[2];
                int resF = boneInfo.get(res);
                if(resF == 0){
                    cout<<"rotate"<<res[0]<<"moveangle"<<res[1]<<"movedis"<<res[2]<<endl;
                    memcpy(wdata.meta.mode3.rotationAngle,&res[0],sizeof(res[0]));
                    memcpy(wdata.meta.mode3.deflection,&res[1],sizeof(res[1]));
                    memcpy(wdata.meta.mode3.distance,&res[2],sizeof(res[2]));
                    cout<<"rotate"<<wdata.meta.mode3.rotationAngle<<"moveangle"<<wdata.meta.mode3.deflection<<"movedis"<<wdata.meta.mode3.distance<<endl;
                //}
            }
//        }else{
//            cout << "Video0 is not online" << endl;
//            videoused = true;
//        }
    pause();
    }
}

