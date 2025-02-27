#pragma once 

#include "Parameter.h"
#include "noncopyable.h"

namespace netco
{
struct MemBlockNode
{
    union 
    {
        MemBlockNode* next; // free memory block
        char data;  // user data
    };
// 每次可以从内存池中获取objSize大小的内存块
template<size_t objSize>
class MemPool : public noncopyable
{
public:
    MemPool()
        : freeListHead_(nullptr)
        , mallocListHead_(nullptr)
        , mallocTimes_(0) 
    {
        if (objSize < sizeof(MemBlockNode))
        {
            objSize_ = sizeof(MemBlockNode);
        }
        else
        {
            objSize_ = objSize;
        }
    }
private:
    MemBlockNode* freeListHead_;
    MemBlockNode* mallocListHead_;
    size_t mallocTimes_;
    size_t objSize_;
};
    
    
};
}//end netco
