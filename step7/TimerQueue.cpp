#include"TimerQueue.h"

#include<sys/timerfd.h>
#include<unistd.h>
#include<assert.h>

using namespace std;

#ifndef UINTPTR_MAX
#define UINTPTR_MAX 0xffffffff
#endif // !UINTPTR_MAX


TimerQueue::TimerQueue(EventLoop *loop)
:   _loop(loop), 
    _timerFd(creatTimerFd()),
    _sortedTimers()
{
    //cout<<"_timerFd: "<<_timerFd<<endl;
    _timerChannel = new Channel(_loop, _timerFd),
    _timerChannel->setReadCallBack(std::bind(&TimerQueue::handleRead, this));
    //cout<<"TimerQueue::handleRead(): "<<&TimerQueue::handleRead<<endl;
    _timerChannel->enableReading();
}

void TimerQueue::handleRead()
{
    //cout<<"TimerQueue::handleRead: "<<TimeStamp::now().toString()<<endl;
    TimeStamp now(TimeStamp::now());
    readTimerFd(_timerFd, now);

    vector<Entry> expired = getExpiredTimer(now);
    //cout<<"expired: "<<expired.size()<<endl;
    for (vector<Entry>::iterator it = expired.begin(); it!=expired.end(); ++it)
    {
        //cout<<"it->second->run();"<<endl;
        it->second->run();
    }

    resetTimers(expired, now);
    
    //reset(expired, now);
}

// 

void TimerQueue::resetTimers(const vector<Entry>& expired, TimeStamp now)
{
    TimeStamp nextExpired;
    for(auto it : expired)
    {
        ActiveTimer timer(it.second, it.second->getTimerId());
        if(it.second->isRepeat() && _cancelingTimers.find(timer)==_cancelingTimers.end())
        {
            it.second->restart(now);
            insert(it.second);
        }
        else
        {
            delete it.second;
        }
    }
    if(!_sortedTimers.empty()){
        nextExpired = _sortedTimers.begin()->second->getCallTime();
    }
    //cout<<"nextExpired: "<<nextExpired.getMicroSecondsSinceEpoch()<<endl;
    if(nextExpired.getMicroSecondsSinceEpoch()>0)
    {
        resetTimerFd(_timerFd, nextExpired);
    }

}

struct timespec TimerQueue::howMuchTimeFromNow(TimeStamp when)
{
    int64_t us = when.getMicroSecondsSinceEpoch()-TimeStamp::now().getMicroSecondsSinceEpoch();;
    if(us<100){
        us=100;
    }
    struct timespec ts;
    //TimeStamp::microSecondsPerSecond
    ts.tv_sec = static_cast<time_t>(us/TimeStamp::microSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((us%TimeStamp::microSecondsPerSecond)*1000);
    //cout<<"TimerQueue::howMuchTimeFromNow: "<<ts.tv_sec<<", "<<ts.tv_nsec<<endl;
    return ts;
}

void TimerQueue::resetTimerFd(int timerfd, TimeStamp callTime)
{
    struct itimerspec newVal;
    struct itimerspec oldVal;
    memset((void*)&newVal, 0, sizeof(newVal));
    memset((void*)&oldVal, 0, sizeof(oldVal));
    newVal.it_value = howMuchTimeFromNow(callTime);
    //cout<<"ret: "<<newVal.it_value.tv_sec<<std::endl;
    // newVal.it_value.tv_sec = 1;
    //newVal.it_value.tv_nsec =0;
    // newVal.it_interval.tv_sec = 1;
    // newVal.it_interval.tv_nsec = 0;
    
    int ret = ::timerfd_settime(timerfd, 0, &newVal, NULL);
    //cout<<"ret: "<<newVal.it_value.tv_sec<<std::endl;
    assert(ret==0);
}



vector<TimerQueue::Entry> TimerQueue::getExpiredTimer(TimeStamp now)
{
    assert(_sortedTimers.size()==_activeTimers.size());
    vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    // what is the compare function of std::pair???
    // compares the first elements, if they are equivalent, 
    // compares the second elements, if those are equivalent, 
    // compares the third elements, and so on.
    // so this `end` will return the first Entry its `callTime` greater than `now` 
    // 1 ad1
    // 1 ad2
    // 2 ad3 
    TimerList::iterator end = _sortedTimers.lower_bound(sentry);
    //cout<<"TimerQueue::getExpiredTimer: "<<TimeStamp::now().toString()<<", "<<end->first.toString()<<", "<<_sortedTimers.begin()->first.toString()<<endl;
    
    assert(end==_sortedTimers.end()||now<end->first);
    copy(_sortedTimers.begin(), end, back_inserter(expired));
    _sortedTimers.erase(_sortedTimers.begin(), end);

    for(const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->getTimerId());
        // how std::set erase a pair ???
        size_t n = _activeTimers.erase(timer);
        assert(n==1);
    }
    
    assert(_sortedTimers.size()==_activeTimers.size());
    return expired;
}

bool TimerQueue::insert(Timer* timer)
{
    TimeStamp callTime = timer->getCallTime();
    //cout<<"b callTime.toString(): "<<callTime.toString()<<endl;
    // callTime = TimeStamp::addTime(callTime, 0.01);
    // cout<<"a callTime.toString(): "<<callTime.toString()<<endl;
    TimerList::iterator it = _sortedTimers.begin();
    bool earliestChanged = false;

    if(it==_sortedTimers.end()|| callTime < it->first)
    {
        earliestChanged = true;
    }
    //cout<<"callTime.toString(): "<<callTime.toString()<<endl;
    {
        // std::set will insert elements in increasing order(callTime)
        std::pair<TimerList::iterator, bool> res = _sortedTimers.insert(Entry(callTime, timer));
        (void) res;
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> res = _activeTimers.insert(ActiveTimer(timer, timer->getTimerId()));
        (void) res;
    }

    return earliestChanged;



}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  //_loop->assertInLoopThread();
  
  bool changed = insert(timer);
    
  if (changed)
  {
    //std::cout<<"TimerQueue::addTimerInLoop: "<<timer->getCallTime().toString()<<", "<<TimeStamp::now().toString()<<endl;
    //cout<<"timer fd: "<<_timerFd<<endl;
    resetTimerFd(_timerFd, timer->getCallTime());
  }
}


int64_t TimerQueue::addTimer(const TimerCallBack& cb, TimeStamp when, double interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    _loop->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return timer->getTimerId();
}

void TimerQueue::readTimerFd(int timerfd, TimeStamp now)
{
    uint64_t timeleft;
    //cout<<"b TimerQueue::readTimerFd: "<<timeleft<<"us left at "<<now.toString()<<endl;
    ssize_t n = ::read(timerfd, &timeleft, sizeof(timeleft));
    (void)n;
    //cout<<"TimerQueue::readTimerFd: "<<timeleft<<"us left at "<<now.toString()<<endl;
}

int TimerQueue::creatTimerFd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    assert(timerfd>=0);
    return timerfd;
}

void TimerQueue::cancel(int64_t timerid)
{
    _loop->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerid));
}

void TimerQueue::cancelInLoop(int timerId)
{
    assert(_sortedTimers.size() == _activeTimers.size());
    ActiveTimer timer(getTimerById(timerId), timerId);
    ActiveTimerSet::iterator it = _activeTimers.find(timer);
    if(it!=_activeTimers.end()){
        size_t n = _sortedTimers.erase(Entry(timer.first->getCallTime(), timer.first));
        assert(n==1);
        delete it->first;
        _activeTimers.erase(it);
    }
    else
    {
        _cancelingTimers.insert(timer);
    }

    assert(_sortedTimers.size() == _activeTimers.size());
}

Timer* TimerQueue::getTimerById(int64_t id)
{
    Timer* res = nullptr;
    for(auto it : _sortedTimers)
    {
        if(it.second->getTimerId()==id) res = it.second;
    }
    assert(res);
    return res;
}









