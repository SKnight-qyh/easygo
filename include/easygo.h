#pragma once
#include "Processor.h"
#include "Logger.h"
#include "Scheduler.h"
namespace easygo
{
    // 协程栈默认大小为2k 
    //tid = -1表示使用调度器分配线程，否则使用tid指定的线程
    void co_go(std::function<void()>& func, size_t stackSize = Parameter::coStackSize, int tid = -1);
    void co_go(std::function<void()>&& func, size_t stackSize = Parameter::coStackSize, int tid = -1);

    // 协程休眠timeout毫秒后继续执行
    void co_sleep(MsTime timeout);

    // 等待调度器结束
    void scheduler_join();
}