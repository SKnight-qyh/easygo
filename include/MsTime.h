#pragma once 

#include <time.h>
#include <stdint.h>

struct timespec;

namespace easygo
{
const char days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
class MsTime
{
public:
    MsTime(int64_t msSinceEpoch )
        : timeVal_(msSinceEpoch)
    {
    }
    MsTime(const MsTime& other)
        : timeVal_(other.timeVal_)
    {
    }
    MsTime(const MsTime&& other)
        : timeVal_(other.timeVal_)
    {
    }

    MsTime& operator=(const MsTime& other)
    {
        this->timeVal_ = other.timeVal_;
        return *this;
    }
    ~MsTime() {};
    // microseconds since 1970/1/1
    static MsTime nowMs();

    // seconds since 1970/1/1
    static time_t nowSec();

    // 根据距离1970-01-01 00:00:00的秒数和与秒数所属时区的时差计算当前时区的时间 
    static void toLocalTime(time_t sec, long timezone, struct tm* tm_time);

    // time interval from now
    timespec timeIntervalFromNow();

    int64_t getTimeVal() 
    {
        return timeVal_;
    }

private:
    int64_t timeVal_;

};

inline bool operator<(MsTime left, MsTime right)
{
    return left.getTimeVal() < right.getTimeVal();
}

inline bool operator>(MsTime left, MsTime right)
{
    return left.getTimeVal() > right.getTimeVal();
}

inline bool operator<=(MsTime left, MsTime right)
{
    return left.getTimeVal() <= right.getTimeVal();
}

inline bool operator>=(MsTime left, MsTime right)
{
    return left.getTimeVal() >= right.getTimeVal();
}

inline bool operator==(MsTime left, MsTime right)
{
    return left.getTimeVal() == right.getTimeVal();
}

inline bool operator!=(MsTime left, MsTime right)
{
    return left.getTimeVal() != right.getTimeVal();
}
} //end easygo