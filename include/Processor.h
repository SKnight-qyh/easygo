#pragma once
#include "noncopyable.h"
#include "MsTime.h"
#include "Coroutine.h"
#include "ObjPool.h"
#include "Context.h"
#include "SpinLock.h"
#include "SpinLockGuard.h"
#include "Epoller.h"
#include "Timer.h"
#include "Parameter.h"

#include <sys/types.h>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
extern thread_local int threadIdx;

namespace netco 
{


enum ProStatus  
{
    PRO_RUNNING = 0,
    PRO_STOPPING,
    PRO_STOPPED
};

class Processor : public noncopyable
{
public:
    Processor(int tid);
    ~Processor();

    // run a new coroutine in this processor
    void goNewCo(std::function<void()>&& func, int stackSize);
    void goNewCo(std::function<void()>& func, int stackSize);
    void yield();

    // the current coroutine waits for timeout ms
    void wait(MsTime timeout);

    // kill the current coroutine
    void killCurCo();

    bool loop();

    void stop();

    void join();

    // wait for an event on the fd
    void waitEvent(int fd, int event);

    inline size_t getCoCnt()
        { return coCnt_; }
    
    inline Coroutine* getCurRunningCo() 
        { return curCo_; }

    inline Context* getMainCtext()
        { return &mainContext_; }

    void goCo(Coroutine* co);

    void goCoBatch(std::vector<Coroutine*>& cos);

    


private:
    // resume the co Coroutine
    void resume(Coroutine* co);

    inline void wakeupEpoller();

    std::thread* pLoop_;
    int status_;     
    int tid_;
    int coCnt_;
    Coroutine* curCo_;
    // main context of this processor
    Context mainContext_;
    Epoller epoller_;
    Timer timer_;

    // 新任务队列 使用双缓存队列 交替读写，防止竞争
    std::queue<Coroutine*> newCos_[2];

    //标记双缓存队列的序号
    volatile int runningNewQue_;

    SpinLock newQueLock_;

    SpinLock coPoolLock_;

    // active events from epoller
    std::vector<Coroutine*> actCos_;

    // coroutines from timer
    std::vector<Coroutine*> timerExpiredCos_;
 
    // 存储kill的协程
    // 一次循环遍历后才会删除
    std::vector<Coroutine*> removeCos_;
    // 当前Processor的注册表 包含所有该Processor管理的协程
    std::unordered_set<Coroutine*> coSet_;

    ObjPool<Coroutine> coPool_;
};// end Processor

}// end netco