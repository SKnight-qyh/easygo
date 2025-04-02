

#pragma once

#include "SpinLock.h"
#include "noncopyable.h"

namespace easygo
{
class SpinLockGuard : public noncopyable
{
public:
    SpinLockGuard(SpinLock& slock)
        : _lock(slock)
    {
        _lock.lock();
    }

    ~SpinLockGuard()
    {
        _lock.unlock();
    }

private:
    SpinLock& _lock;
};  // end SpinLockGuard

}   // end easygo