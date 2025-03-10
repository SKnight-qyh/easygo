#pragma once

#include "noncopyable.h"

#include <atomic>

namespace netco
{
class SpinLock : public noncopyable
{
public:
    SpinLock()
        : _sem(1) 
    { }
    ~SpinLock() 
    { 
        unlock(); 
    }

    void lock()
    {
        int expected = 1;
        while (!_sem.compare_exchange_weak(expected, 0))
        {
            expected = 1;
        }
    }

    void unlock()
    {
        _sem.store(1);
    }



private:
    std::atomic_int _sem;   // 1: unlocked, 0: locked
};  // end SpinLock
}   // end netco 

