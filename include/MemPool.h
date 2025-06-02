#pragma once 

#include "Parameter.h"
#include "noncopyable.h"

#include <cstdlib>

namespace easygo
{
struct MemBlockNode
{
    union 
    {
        MemBlockNode* next; // free memory block
        char data;  // user data
    };
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
    ~MemPool();
    void* allocMemBlock();
    void freeMemBlock(void* block);
private:
    MemBlockNode* freeListHead_;    // 空闲小内存块链表头
    MemBlockNode* mallocListHead_;  // 已分配大内存块链表头
    size_t mallocTimes_;            // 已分配大内存块个数
    size_t objSize_;                // 每个小内存块的大小
};

template<size_t objSize>
MemPool<objSize>::~MemPool()
{
    while (mallocListHead_)
    {
        MemBlockNode* node = mallocListHead_;
        mallocListHead_ = mallocListHead_->next;
        free(static_cast<void*>(node));
    }
}


template<size_t objSize>
void* MemPool<objSize>::allocMemBlock()
{
    void* ret;
    // 如果空闲链表为空，需要重新分配大内存块，需要额外分配一个node管理大内存块
    if (nullptr == freeListHead_)
    {
        size_t mallocCnt = Parameter::memPoolMallocObjCnt + mallocTimes_;
        void* newMallocBlock = malloc(mallocCnt * objSize_ + sizeof(MemBlockNode));
        // 将node放在内存块前面管理大内存 
        MemBlockNode* newNode = static_cast<MemBlockNode*>(newMallocBlock);
        newNode->next = mallocListHead_;
        mallocListHead_ = newNode;
        newMallocBlock = static_cast<char*>(newMallocBlock) + sizeof(MemBlockNode);
        for (int i = 0; i < mallocCnt; ++i)
        {
            MemBlockNode* nextNode = static_cast<MemBlockNode*>(newMallocBlock);
            nextNode->next = freeListHead_;
            freeListHead_ = nextNode;
            newMallocBlock = static_cast<char*>(newMallocBlock) + objSize_;
        }
        ++mallocTimes_;
    }
    ret = &(freeListHead_->data);
    freeListHead_ = freeListHead_->next;
    return ret;
}
// 小内存块归还到内存池中
template<size_t objSize>
void MemPool<objSize>::freeMemBlock(void* block)
{
    if (nullptr == block)
    {
        return ;
    }
    MemBlockNode* newNode = static_cast<MemBlockNode*>(block);
    newNode->next = freeListHead_;
    freeListHead_ = newNode;
}    

}//end easygo
