#pragma once

#include "noncopyable.h"
#include "Context.h"
#include "Epoller.h"
#include "Coroutine.h"
#include "MsTime.h"

#include <time.h>
#include <queue> 
#include <vector>
#include <map>

#define TIMER_DUMMYBUF_SIZE 1024
namespace easygo 
{
class Context;
class Epoller;
class Timer : public noncopyable
{
public:
    using TimeCoroution = typename std::pair<MsTime, Coroutine*>;
    using TimerHeap = typename std::priority_queue<TimeCoroution, std::vector<TimeCoroution>, std::greater<TimeCoroution>>;
    Timer();
    ~Timer();

    // 将timefd注册到epoll中
    bool init(Epoller* pEpoller);


    void getExpiredCos(std::vector<Coroutine*>& expiredCos);
    
    // 在time时刻需要resume co
    void runAt(MsTime time, Coroutine* co);

    // 在time ms后resume co
    void runAfter(MsTime time, Coroutine* co);

    // 唤醒Timer
    void wakeUp();
private:
    int timefd_;

    // 给timefd重新设置绝对时间time
    bool resetTimeOfTimefd(MsTime time);

    inline bool isTimefdUseful()
    {
        return timefd_ >= 0;
    }
    char dummyBuf_[TIMER_DUMMYBUF_SIZE];
    
    // std::multimap<MsTime, Coroutine*> timerCoMap_;
    TimerHeap timerCoHeap_; // mintop heap

};//end timer
}//end easygo