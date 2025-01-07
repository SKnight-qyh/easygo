#pragma once

#include "noncopyable.h"

#include <vector>

namespace netco
{
    class Processor;

    enum scheduleStrategy
    {
        MIN_EVENT_FIRST = 0,    // 最少事件优先
        ROUND_ROBIN          // 轮询    
    };


    class ProcessorSelector : public noncopyable
    {
    public:
        ProcessorSelector(std::vector<Processor*>& processors, int strategy = MIN_EVENT_FIRST);
        ~ProcessorSelector();
        inline void setStrategy(int strategy) 
            { _strategy = strategy; }
        Processor* next();
    private:
        int _curProcessorIdx;
        int _strategy;
        std::vector<Processor*>& _processors;
    };
}