#pragma once
#include "noncopyable.h"
#include "Processor.h"
#include "ProcessorSelector.h"

#include <functional>
#include <mutex>
#include <vector> 

namespace easygo
{
    // 单例模式
    class Scheduler : public noncopyable
    {
    protected:
        Scheduler();
        ~Scheduler();
    public:
        
        static Scheduler* getScheduler();

        // create a new coroutine on thread idx
        void createNewCo(std::function<void()>&& func, size_t stackSize);
        void createNewCo(std::function<void()>& func, size_t stackSize);
        
        Processor* getProcessor(int id);

        int getProCnt();

        void join();
    private:

        // 初始化Scheduler,并开启threadCount个线程
        bool startScheduler(int threadCnt);

        // 
        static Scheduler* _pScheduler;

        // 为了保证服务器效率，不能长时间占用mutex
        static std::mutex _schMutex;

        std::vector<Processor*> _pros;
 
        ProcessorSelector _proSeletor;



    };
}