#include "../include/Scheduler.h"


#include <sys/sysinfo.h>

using namespace netco;

Scheduler* Scheduler::_pScheduler = nullptr;
std::mutex Scheduler::_schMutex;

Scheduler::Scheduler()
    : _proSeletor(_pros)
{ }

// stop all processors
Scheduler::~Scheduler()
{
    for (auto pro : _pros)
    {
        pro->stop();    // 设置状态值为PRO_STOPPING
    }
    for (auto pro : _pros)
    {
        pro->join();    // 调用thread的join()
        delete pro;
    }
}

bool Scheduler::startScheduler(int threadCnt)
{
    for (int i = 0; i < threadCnt; ++i)
    {
        auto pro = new Processor(i);
        _pros.emplace_back(pro);
        pro->loop();
    }
    return true;
}

Scheduler* Scheduler::getScheduler()
{
    // 双重检查锁定 
    if (!_pScheduler)   // 防止不必要的加锁
    {
        std::lock_guard<std::mutex> lock(_schMutex);  // 线程安全保证唯一实例
        if (!_pScheduler)
        {
            _pScheduler = new Scheduler();
            _pScheduler->startScheduler(::get_nprocs_conf());    // 取系统配置的处理器数量
        }
    }
    return _pScheduler;
}


void Scheduler::createNewCo(std::function<void()>&& func, size_t stackSize)
{
    _proSeletor.next()->goNewCo(std::move(func), stackSize);
}

void Scheduler::createNewCo(std::function<void()>& func, size_t stackSize)
{
    _proSeletor.next()->goNewCo(func, stackSize);
}

Processor* Scheduler::getProcessor(int id)
{
    return _pros[id];
}

int Scheduler::getProCnt()
{
    return _pros.size();
}

void Scheduler::join()
{
    for (auto pro : _pros)
    {
        pro->join();
    }
}