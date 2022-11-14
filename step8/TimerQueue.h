#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include<set>
#include<vector>

#include"CallBacks.h"
#include"Channel.h"
#include"TimeStamp.h"
#include"Timer.h"
#include"EventLoop.h"

class Channel;
class EventLoop;
class TimeStamp;

class TimerQueue
{
public:
    //class Channel;
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    int64_t addTimer(const TimerCallBack& cb, TimeStamp when, double interval);
    void addTimerInLoop(Timer *timer);
    void cancelById(int id);
    int getNumTimer(){return _sortedTimers.size();}

    struct timespec howMuchTimeFromNow(TimeStamp when);


private:
    typedef std::pair<TimeStamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    
    void cancelInLoop(int timerId);

    int creatTimerFd();
    void readTimerFd(int timerfd, TimeStamp now);

    std::vector<TimerQueue::Entry> getExpiredTimer(TimeStamp now);
    void resetTimers(const std::vector<Entry>& expired, TimeStamp now);

    void handleRead();

    bool insert(Timer* timer);
    void cancel(int64_t timerid);
    void reset(const std::vector<Entry>& expired, TimeStamp now);
    void resetTimerFd(int timerfd, TimeStamp callTime);

    Timer* getTimerById(int64_t id);

    

    EventLoop *_loop;
    const int _timerFd;
    Channel *_timerChannel;


    TimerList _sortedTimers;
    ActiveTimerSet _activeTimers;
    ActiveTimerSet _cancelingTimers;


};

#endif // !TIMERQUEUE_H


