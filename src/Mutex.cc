#include "../include/Mutex.h"

using namespace easygo;

void RWMutex::freeLock()
{
    status_ = MU_FREE;
    while (!waitingCo_.empty())
    {
        auto wakeCo = waitingCo_.front();
        waitingCo_.pop();
        wakeCo->getProcessor()->goCo(wakeCo);
    }
    
}


void RWMutex::RLock()
{
    while (true)
    {
        {
            SpinLockGuard guard(lock_);
            if (status_ != MU_WRITING)
            {
                readingNum_.fetch_add(1);
                status_ = MU_READING;
                return ;
            }
            // 如果锁被写者占用，将当前协程加入等待队列,并让出执行权
            waitingCo_.emplace(Scheduler::getScheduler()->getProcessor(threadIdx)->getCurRunningCo());
        }
        //如果新调度的协程也需要获取 lock_（例如调用 WLock()），它会因为锁被当前线程持有而无法继续，形成死锁。
        Scheduler::getScheduler()->getProcessor(threadIdx)->yield();
    }

}

void RWMutex::RUnlock()
{
    SpinLockGuard guard(lock_);
    auto ret = readingNum_.fetch_add(-1);   // 返回值是之前的 readingNum_ 值
    if (ret == 1)
    {
        freeLock();
    }

}

void RWMutex::WLock()
{
    while (true)
    {
        {
            SpinLockGuard guard(lock_);
            if ((status_ == MU_FREE))
            {
                status_ = MU_WRITING;
                return ;
            }
            // 如果锁被其他线程占用，将当前协程加入等待队列,并让出执行权
            waitingCo_.emplace(Scheduler::getScheduler()->getProcessor(threadIdx)->getCurRunningCo());
        }
        Scheduler::getScheduler()->getProcessor(threadIdx)->yield();
    }

}

void RWMutex::WUnlock()
{
    SpinLockGuard guard(lock_);
    freeLock();
}