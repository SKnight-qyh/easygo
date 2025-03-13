#include "../include/Processor.h"
#include "../include/Coroutine.h"

using namespace netco;

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

// size_t Processor::getCoCnt() const
// {
//     return 0;
// }
void Processor::stop()
{
    status_ = PRO_STOPPING;
}

void Processor::join()
{
    pLoop_->join();
}
// processor进行epoll-loop循环
bool Processor::loop()  
{

}