//
// Created by fatcat on 2/21/19.
//

#include "BoneInfo.h"
using namespace std;

void BoneInfo::set(double rotateangle, double moveangle, double movedistance,unsigned char flag) {
    lock_guard<mutex> l(info_mutex);
    this->rotateangle = rotateangle;
    this->moveangle = moveangle;
    this->movedistance = movedistance;
    this->flag = flag;
    this->used = false;
}

char BoneInfo::get(double *res) {
    lock_guard<mutex> l(info_mutex);
    if (used) {
        return -1;
    } else {
        res[0] = rotateangle;
        res[1] = moveangle;
        res[2] = movedistance;
        used = true;
        return flag;
    }
}

