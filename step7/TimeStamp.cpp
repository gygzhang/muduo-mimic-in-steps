#include "TimeStamp.h"
#include<sys/time.h>
#include<assert.h>

#include <inttypes.h>


using namespace std;

TimeStamp::TimeStamp()
: _usSinceEpoch(0)
{

}

TimeStamp::TimeStamp(int64_t ms)
: _usSinceEpoch(ms)
{
    
}

TimeStamp TimeStamp::addTime(TimeStamp timestamp, double seconds)
{
    int64_t us = static_cast<int64_t>(seconds * TimeStamp::microSecondsPerSecond);
    return TimeStamp(timestamp.getMicroSecondsSinceEpoch() + us);
}

int64_t TimeStamp::getMicroSecondsSinceEpoch()
{
    return _usSinceEpoch;
}

string TimeStamp::toString() const
{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(_usSinceEpoch/microSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);
    int64_t microseconds = _usSinceEpoch % microSecondsPerSecond;
    snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

TimeStamp TimeStamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    //assert(seconds*microSecondsPerSecond==tv.tv_usec);
    return TimeStamp(seconds*microSecondsPerSecond+tv.tv_usec);
}





