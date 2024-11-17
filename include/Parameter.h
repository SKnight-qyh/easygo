#pragma once

#include <sys/types.h>

namespace netco
{
    namespace  Parameter
    {
        // the size of coroutine stack
        const static size_t coStackSize = 1024 * 32;

        // the size of active epoll_event vector
        static constexpr int epollerListFirstSize = 16;

        // timeout for epoll_wait
        static constexpr int epollTimeoutMs = 1000;

        // the size of listen socket backlog
        static constexpr int listenBacklog = 4096;

        // 内存池没有空闲内存块是申请memPoolMallocObjCnt个对象大小的内存块
        static constexpr int memPoolMallocObjCnt = 40;

    }
}