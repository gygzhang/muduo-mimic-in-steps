#include"Timer.h"

std::atomic<int64_t> Timer::_numTimer;

Timer::Timer(const TimerCallBack& cb, TimeStamp when, double interval)
:   _timerCallBack(std::move(cb)), 
    _callTime(when),
    _interval(interval),
    _timerId(_numTimer.fetch_add(1)),
    _repeat(interval>0.0? true:false)
{

}

void Timer::restart(TimeStamp now)
{
    if(_repeat)
    {
        _callTime = TimeStamp::addTime(now, _interval);
    }else
    {
        _callTime = [](){return TimeStamp();}();
    }
}

void Timer::run() const
{
    _timerCallBack();
}

bool Timer::isRepeat()
{
    return _repeat;
}

