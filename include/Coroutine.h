#pragma once

#include "noncopyable.h"
#include "Context.h"

#include <functional> 

namespace netco
{
class Processor;

enum coStatus
{
    CO_READY = 0,
    CO_RUNNING,
    CO_WAITTING,
    CO_DEAD
};

class Coroutine : public noncopyable
{
public:
// 有依赖的proccessor, 上下文的栈, 以及运行的func
    Coroutine(Processor* pro, size_t stavkSize, std::function<void()>&& func);
    Coroutine(Processor* pro, size_t stackSize, std::function<void()>& func);
    ~Coroutine();

    // 恢复协程
    void resume(); 

    // 挂起协程
    void yield();

    inline Processor* getProcessor() 
    { return processor_; }

    inline void startFunc()
    { func_(); }

    inline Context* getcontext()
    { return &context_; }
private:
    Processor* processor_;
    Context context_;
    std::function<void()> func_;
    coStatus status_;

};
}   // end netco