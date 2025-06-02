#include "../include/Processor.h"


using namespace easygo;

// 线程的局部变量， 不同线程间互不影响
thread_local int threadIdx = -1;

Processor::Processor(int tid)
    : tid_(tid)
    , pLoop_(nullptr)
    , status_(PRO_STOPPED)
    , runningNewQue_(0)
    , curCo_(nullptr)
    , mainContext_(0)
{
    mainContext_.makeCurContext();

}

Processor::~Processor()
{
    if (PRO_RUNNING == status_)
    {
        stop();
    }
    if (PRO_STOPPING == status_)
    {
        join();
    }
    if (nullptr != pLoop_)
    {
        delete pLoop_;
    }

    for (auto& co : coSet_)
    {
        delete co;
    }
}


void Processor::stop()
{
    status_ = PRO_STOPPING;
}

void Processor::join()
{
    pLoop_->join();
}

void Processor::resume(Coroutine* co)
{
    if (nullptr == co)
    {
        return ;
    }
    if (coSet_.find(co) != coSet_.end())
    {
        curCo_ = co;
        co->resume();
    }
}

void Processor::wakeupEpoller()
{
    timer_.wakeUp();
}

void Processor::yield()
{
    if (nullptr == curCo_)
    {
        return ;
    }
    curCo_->yield();
    mainContext_.swapToMe(curCo_->getcontext());
}

void Processor::wait(MsTime timeout)
{
    curCo_->yield();
    timer_.runAfter(timeout, curCo_);
    mainContext_.swapToMe(curCo_->getcontext());
}
// processor进行epoll-loop循环
// 先处理超时，再处理新到达的协程，最后处理epoller唤醒的协程
bool Processor::loop()  
{
    if (!epoller_.init())
    {
        return false;
    }
    if (!timer_.init(&epoller_))
    {
        return false;
    }
    pLoop_ = new std::thread(
        [this]
    {
        threadIdx = tid_;
        status_ = PRO_RUNNING;
        while (status_ == PRO_RUNNING)
        {
            actCos_.clear();
            timerExpiredCos_.clear();
            // 获取活跃事件
            epoller_.getActiveEpollEvents(Parameter::epollTimeoutMs, actCos_);
            // 获取超时事件
            timer_.getExpiredCos(timerExpiredCos_);
            for (auto& co : timerExpiredCos_)
            {
                resume(co);
            }
            // 处理新到达的协程
            Coroutine* newCo = nullptr;
            int runningNewQueIdx = runningNewQue_;
            while (!newCos_[runningNewQueIdx].empty())
            {
                newCo = newCos_[runningNewQueIdx].front();
                newCos_[runningNewQueIdx].pop();
                coSet_.insert(newCo);
            }
            // 加锁防止线程竞争,加作用域自动释放锁
            {
                SpinLockGuard lock(newQueLock_);
                runningNewQue_ = !runningNewQueIdx;
            }

            // 处理已经唤醒的协程
            for (auto& co : actCos_)
            {
                resume(co);
            }
            
            // 处理kill的进程
            if (!removeCos_.empty())
            {
                SpinLockGuard lock(coPoolLock_);
                for (auto& co : removeCos_)
                {
                    coSet_.erase(co);
                    coPool_.deleteObj(co);
                }
                removeCos_.clear();
            }

        }
        status_ = PRO_STOPPED;
    });
    return true;
}

void Processor::killCurCo()
{
    if (nullptr == curCo_)
    {
        return ;
    }
    yield();
    removeCos_.emplace_back(curCo_);
}

// removeEvent防止协程被重复唤醒
void Processor::waitEvent(int fd, int event)
{
    epoller_.addEvent(curCo_, fd, event);
    yield();    // 让出控制权给loop
    epoller_.removeEvent(curCo_, fd, event);
}


void Processor::goCo(Coroutine* co)
{
    if (nullptr == co)
    {
        return ;
    }
    {
        SpinLockGuard lock(newQueLock_);
        newCos_[!runningNewQue_].emplace(co);
    }
    wakeupEpoller();

}

void Processor::goCoBatch(std::vector<Coroutine*>& cos)
{
    if (cos.empty())
    {
        return ;
    }
    {
        SpinLockGuard lock(newQueLock_);
        for (auto& co : cos)
        {
            newCos_[!runningNewQue_].emplace(co);
        }
    }
    wakeupEpoller();
}

void Processor::goNewCo(std::function<void()>& func, int stackSize)
{
    // Coroutine newCo(this, stackSize, func);
    // 使用对象池
    Coroutine* newCo = nullptr;
    {
        SpinLockGuard lock(coPoolLock_);
        newCo = coPool_.newObj(this, stackSize, func);
    }
}

void Processor::goNewCo(std::function<void()>&& func, int stackSize)
{
    // Coroutine newCo(this, stackSize, func);
    // 使用对象池
    Coroutine* newCo = nullptr;
    {
        SpinLockGuard lock(coPoolLock_);
        newCo = coPool_.newObj(this, stackSize, std::move(func));
    }
}