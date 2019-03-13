//
// Created by fatcat on 2/21/19.
//

#include "Threadinfo.h"

using namespace std;

bool ThreadInfo::getThreadState() {
    lock_guard<mutex> l(ts_mutex);
    return this->threadState;
}

void ThreadInfo::setThreadState(bool state) {
    lock_guard<mutex> l(ts_mutex);
    this->threadState = state;
}

void ThreadInfo::init() {
    lock_guard<mutex> l(info_mutex);
    lock_guard<mutex> s(ts_mutex);
    this->used = true;
    this->threadState = true;
}