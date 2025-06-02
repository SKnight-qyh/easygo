
#include "../include/Context.h"
#include "../include/Parameter.h"

#include <stdlib.h>
using namespace easygo;

Context::Context(size_t stackSize)
    : stackSize_(stackSize)
    , pStack_(nullptr)
{
}
Context::~Context()
{
    if (nullptr != pStack_)
    {
        free(pStack_);
    }
}

void Context::makeContext(void (*func)(), Processor* pro, Context* nextContext)
{
    if (nullptr == pStack_)
    {
        pStack_ = malloc(stackSize_);
    }
    makeCurContext();
    // 如果不设置新stack， func会在旧的stack上执行
    context_.uc_stack.ss_sp = pStack_;
    context_.uc_stack.ss_size = Parameter::coStackSize;
    context_.uc_stack.ss_flags = 0;
    context_.uc_link = nextContext->getCurUcontext();
    ::makecontext(&context_, func, 1, pro); // func 会在新设置的stack

}

void  Context::makeCurContext()
{
    ::getcontext(&context_);
}

void Context::swapToMe(Context* pOldContext)
{
    if (nullptr == pOldContext)
    {
        ::setcontext(&context_);
    }
    else
    {
        swapcontext(pOldContext->getCurUcontext(), &context_);
    }
}