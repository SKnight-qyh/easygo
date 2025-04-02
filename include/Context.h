#pragma once 

// #include "noncopyable.h"

#include <ucontext.h>

namespace easygo 
{

class Processor;
// 允许拷贝构造
class Context
{
public:
    Context(size_t stackSize);
    ~Context();
    Context(const Context& other)
        : context_(other.context_)
        , pStack_(other.pStack_)
    {
    }

    Context& operator=(const Context& ) = delete;
    
    // 保存当前的上下文
    void makeCurContext();

    // 调用makeCurContext()，保存当前上下文，并切换到func指定的函数，Pro作为func的参数传入
    void makeContext(void (*func)(), Processor*, Context* nextContext);

    
    
    // 将当前上下文保存到oldContext中，并切换到当前上下文
    // oldContext为nullptr时，直接运行
    void swapToMe(Context* pOldContext);

    inline ucontext_t* getCurUcontext()
    { return &context_;}
private:
    struct ucontext_t context_;
    void* pStack_;
    size_t stackSize_;


}; // end context
}   // end easygo 