//
// Created by fatcat on 2/21/19.
//

#ifndef MULTITHREADINGTEST_THREADINFO_H
#define MULTITHREADINGTEST_THREADINFO_H

#include <mutex>
#include <iostream>

class ThreadInfo {
protected:
    //is info used?
    bool used = true;
    std::mutex info_mutex;
    //run:true finish:false
    bool threadState = true;
    std::mutex ts_mutex;
public:

    bool getThreadState();

    void setThreadState(bool state);

    void init();
};

#endif //MULTITHREADINGTEST_THREADINFO_H
