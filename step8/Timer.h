#ifndef TIMER_H
#define TIMER_H

#include"TimeStamp.h"
#include"CallBacks.h"

#include<atomic>

class TimeStamp;

class Timer
{
public:
    Timer(const TimerCallBack& cb, TimeStamp when, double interval);
    ~Timer()=default;

    void run() const;

    void restart(TimeStamp now);
    bool isRepeat();

    static int64_t getNumTimer(){return _numTimer;}
    
    int64_t getTimerId(){return _timerId;}

    TimeStamp getCallTime(){return _callTime;}
    
    


private:
    const TimerCallBack _timerCallBack;
    TimeStamp _callTime;
    const double _interval;
    const bool _repeat;
    const int64_t _timerId;

    static std::atomic<int64_t> _numTimer;

};


#endif // !TIMER_H


