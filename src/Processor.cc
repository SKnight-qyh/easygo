#include "../include/Processor.h"


using namespace netco;

// 线程的局部变量， 不同线程间互不影响
thread_local int threadIdx = -1;

Processor::Processor(int tid)
    : tid_(tid)
{ }

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