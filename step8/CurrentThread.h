#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include<unistd.h>  //  __thread
#include<sys/syscall.h> //  syscall()
#include "TimeStamp.h"

namespace CurrentThread
{
    inline __thread int _cachedThreadID;
    inline void cachedThreadID()
    {
        _cachedThreadID = static_cast<int>(::syscall(SYS_gettid));
    }

    inline int getThreadID()
    {
        if(_cachedThreadID==0) cachedThreadID();
        return _cachedThreadID;
    }

    inline void sleep(int us)
    {
        struct timespec ts = { 0, 0 };
        ts.tv_sec = static_cast<time_t>(us / TimeStamp::microSecondsPerSecond);
        ts.tv_nsec = static_cast<long>(us % TimeStamp::microSecondsPerSecond * 1000);
        ::nanosleep(&ts, NULL);
    }
}

#endif // !CURRENTTHREAD_H




