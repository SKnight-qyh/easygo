#include "../include/ProcessorSelector.h"
#include "../include/Processor.h"
using namespace easygo;

ProcessorSelector::ProcessorSelector(std::vector<Processor*>& processors, int strategy)
    : _processors(processors)
    , _strategy(strategy)
    , _curProcessorIdx(0)
{
}

ProcessorSelector::~ProcessorSelector() {}

Processor* ProcessorSelector::next()
{
    int n = static_cast<int>(_processors.size());
    if (n == 0)
        return nullptr;
    int minCoProcessorIdx = 0;
    size_t minCoCnt = _processors.front()->getCoCnt();
    switch(_strategy)
    {
    case MIN_EVENT_FIRST:
        for (int i = 1; i < n; ++i)
        {
            size_t coCnt = _processors[i]->getCoCnt();
            if (coCnt < minCoCnt)
            {
                minCoCnt = coCnt; 
                minCoProcessorIdx = i;
            }
        }
        _curProcessorIdx = minCoProcessorIdx;
        break;
    case ROUND_ROBIN:
        ++_curProcessorIdx;
        if (_curProcessorIdx >= n)
        {
            _curProcessorIdx = 0;
        }
        break;
    default:
        break;
    }
    
    return _processors[_curProcessorIdx];
}