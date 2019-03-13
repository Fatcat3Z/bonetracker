//
// Created by fatcat on 2/21/19.
//

#ifndef MULTITHREADINGTEST_BONEINFO_H
#define MULTITHREADINGTEST_BONEINFO_H

#include <mutex>
#include "Threadinfo.h"


class BoneInfo :public ThreadInfo{
public:
    void set(double rotateangle,
             double moveangle,
             double movedistance,
             unsigned char flag);

    char get(double *res);

private:
    double rotateangle;
    double moveangle;
    double movedistance;
    unsigned char flag = 0;

};
#endif //MULTITHREADINGTEST_BONEINFO_H
