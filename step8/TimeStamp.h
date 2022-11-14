#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include<ctime>
#include<string>
// #include<operators>

class TimeStamp
{
public:
    typedef signed long int int64_t;
    TimeStamp();
    TimeStamp(int64_t ms);
    ~TimeStamp()=default;

    
    int64_t getMicroSecondsSinceEpoch();

    std::string toString() const;
    
    friend bool operator<(const TimeStamp& lhs, const TimeStamp& rhs)
    {
        return lhs._usSinceEpoch<rhs._usSinceEpoch;
    }

    friend bool operator==(const TimeStamp& lhs, const TimeStamp& rhs)
    {
        return lhs._usSinceEpoch==rhs._usSinceEpoch;
    }

    static TimeStamp now();
    static TimeStamp addTime(TimeStamp timestamp, double seconds);
    static const int microSecondsPerSecond = 1000*1000;

private:
    int64_t _usSinceEpoch;
    
};

#endif



