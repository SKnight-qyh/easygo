#include "../include/Coroutine.h"
#include "../include/Processor.h"
#include "../include/Context.h"

using namespace easygo;

static void coFunc(Processor* pro)
{
    pro->getCurRunningCo()->startFunc();
    pro->killCurCo();
}
Coroutine::Coroutine(Processor* pro, size_t stackSize, std::function<void()>&& func) // 调用时传入的对象是右值，在函数体内是左值
    : processor_(pro)
    , func_(std::move(func))
    , status_(CO_DEAD)
    , context_(stackSize)
{
    status_ = CO_READY; // 为什么直接CO_READY？
                        // 防止context_构造失败
}

Coroutine::Coroutine(Processor* pro, size_t stackSize, std::function<void()>& func) // 调用时传入的对象是右值，在函数体内是左值
    : processor_(pro)
    , func_(func)
    , status_(CO_DEAD)
    , context_(stackSize)
{
    status_ = CO_READY; // 为什么直接CO_READY？
                        // 防止context_构造失败
}

Coroutine::~Coroutine()
{ 
}

void Coroutine::resume()
{
    Context* pMainContext = processor_->getMainCtext();
    switch (status_)
    {
        case CO_READY:
            status_ = CO_RUNNING;
            context_.makeContext((void (*)(void)) coFunc, processor_, pMainContext);
            context_.swapToMe(pMainContext);
            break;
        case CO_WAITTING:
            status_ = CO_RUNNING;
            context_.swapToMe(pMainContext);
            break;
        default:
            break;
    }
}



void Coroutine::yield()
{
    status_ = CO_WAITTING;
}