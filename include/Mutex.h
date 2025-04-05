#pragma once

#include "SpinLock.h"
#include "SpinLockGuard.h"
#include "Coroutine.h"
#include "noncopyable.h"
#include "Processor.h"
#include "Scheduler.h"

#include <atomic>
#include <queue>


namespace easygo
{
    enum muStatus
    {
        MU_FREE = 0,
        MU_READING,
        MU_WRITING
    };

    class RWMutex
    {
    public:
        RWMutex()
            : status_(MU_FREE)
            , readingNum_(0)
        {}
        ~RWMutex() {};
        
        // 读锁相互不互斥，与写锁互斥
        void RLock();
        void RUnlock();

        // 写锁相互互斥， 与读锁互斥
        void WLock();
        void WUnlock();

    private:
        
        void freeLock();

        muStatus status_;
        SpinLock lock_;
        std::atomic<int> readingNum_;
        std::queue<Coroutine*> waitingCo_;

    }
};