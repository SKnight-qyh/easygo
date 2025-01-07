

#pragma once

#include "spinLock.h"
#include "noncopyable.h"

namespace netco
{
class spinLockGuard : public noncopyable
{
public:
    spinLockGuard(spinLock& slock)
        : _lock(slock)
    {
        _lock.lock();
    }

    ~spinLockGuard()
    {
        _lock.unlock();
    }

private:
    spinLock& _lock;
};  // end spinLockGuard

}   // end netco