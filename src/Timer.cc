#include "../include/Timer.h"

#include <unistd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <string.h>
using namespace netco;

Timer::Timer()
    : timefd_(-1)
{
}

Timer::~Timer()
{
    if (isTimefdUseful())
    {
        ::close(timefd_);
    }
}

bool Timer::init(Epoller* pEpoller)
{   // 单调时钟，从某个固定点开始计时，不受系统时间调整的影响
    timefd_ = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (isTimefdUseful())
    {
       return pEpoller->addEvent(nullptr, timefd_, EPOLLIN | EPOLLPRI | EPOLLRDHUP);
    }
    return false;
}

// 检查过期协程 → 清空触发计数 → 重置下次到期时间。
void Timer::getExpiredCos(std::vector<Coroutine*>& expiredCos)
{
    MsTime nowTime = MsTime::nowMs();
    while (!timerCoHeap_.empty() && timerCoHeap_.top().first <=  nowTime)
    {
        expiredCos.emplace_back(timerCoHeap_.top().second);
        timerCoHeap_.pop();
    }
    if (!expiredCos.empty())
    {
        ssize_t cnt = TIMER_DUMMYBUF_SIZE;
        while (cnt >= TIMER_DUMMYBUF_SIZE)  // 清空timefd的触发计数，并不是读取数据
        {
            cnt = ::read(timefd_, &dummyBuf_, TIMER_DUMMYBUF_SIZE);
        }
    } 

    if (!timerCoHeap_.empty())
    {
        MsTime nextTime = timerCoHeap_.top().first;
        resetTimeOfTimefd(nextTime);
    }

}

bool Timer::resetTimeOfTimefd(MsTime time)
{
    struct itimerspec newVal;
    struct itimerspec oldVal;
    ::memset(&newVal, 0, sizeof(newVal));
    ::memset(&oldVal, 0, sizeof(oldVal));
    newVal.it_value = time.timeIntervalFromNow();
    // flags = 0 表示相对时间，即从当前时间开始计时
    int ret = ::timerfd_settime(timefd_, 0, &newVal, &oldVal);
    return ret == 0;

}

void Timer::wakeUp()
{
    resetTimeOfTimefd(MsTime::nowMs());
}

void Timer::runAt(MsTime time, Coroutine* co)
{
    timerCoHeap_.emplace(time, co);
    
    // 最先添加的co是最早到期，所以需要重置timefd的到期时间
    if (timerCoHeap_.top().first == time)
    {
        resetTimeOfTimefd(time);
    }
}

void Timer::runAfter(MsTime time, Coroutine* co)
{
    MsTime setTime(MsTime::nowMs().getTimeVal() + time.getTimeVal());
    runAt(setTime, co);
}