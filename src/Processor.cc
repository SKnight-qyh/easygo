#include "../include/Processor.h"


using namespace netco;

Processor::Processor(int id)
    : _id(id)
{ }
// size_t Processor::getCoCnt() const
// {
//     return 0;
// }
void Processor::stop()
{
    _status = PRO_STOPPING;
}

void Processor::join()
{
    _pLoop->join();
}
// processor进行epoll-loop循环
void Processor::loop()  
{

}