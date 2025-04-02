#include "../include/easygo.h"

#include <sys/sysinfo.h>
using namespace easygo;

void easygo::co_go(std::function<void()>& func, size_t stackSize = Parameter::coStackSize, int tid)
{
    if (tid >= ::get_nprocs_conf())
    {
        LOG_ERROR("the tid(%d) is out of range\n", tid);
        return;
    }
    else if (tid < 0)
    {
        Scheduler::getScheduler()->createNewCo(func, stackSize);
    }
    else
    {
        Scheduler::getScheduler()->getProcessor(tid)->goNewCo(func, stackSize);
    }

}
void easygo::co_go(std::function<void()>&& func, size_t stackSize = Parameter::coStackSize, int tid)
{
    if (tid >= ::get_nprocs_conf())
    {
        LOG_ERROR("the tid(%d) is out of range\n", tid);
        return;
    }
    else if (tid < 0)
    {
        Scheduler::getScheduler()->createNewCo(std::move(func), stackSize);
    }
    else
    {
        Scheduler::getScheduler()->getProcessor(tid)->goNewCo(std::move(func), stackSize);
    }

}

void easygo::co_sleep(MsTime timeout)
{
    Scheduler::getScheduler()->getProcessor(threadIdx)->wait(timeout);
}

void easygo::scheduler_join()
{
    Scheduler::getScheduler()->join();
}

