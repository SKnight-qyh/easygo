#include "../include/Epoller.h"
#include "../include/Parameter.h"

#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

using namespace easygo;

Epoller::Epoller()
    : epollfd_(-1)
    , activeEpollEvents_(Parameter::epollerListFirstSize)
{
}

Epoller::~Epoller()
{
    if (isEpollfdUseful())
    {
        ::close(epollfd_);
    }
}

bool Epoller::init()
{
    epollfd_ = epoll_create1(EPOLL_CLOEXEC);//使用EPOLL_CLOEXEC标志，这样在fork()调用中，子进程不会继承epoll句柄，避免了fd泄露问题。
    assert(epollfd_ >= 0);
    return isEpollfdUseful();

}


bool Epoller::addEvent(Coroutine* co, int fd, int event)
{
    if (!isEpollfdUseful())
    {
        return false;
    }
    struct epoll_event ev;
    memset (&ev, 0, sizeof ev);
    ev.data.ptr  = co;
    ev.events = event;
    int ret = ::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
    assert(ret == 0);
    return true;
}

bool Epoller::removeEvent(Coroutine* co, int fd, int event)
{
    if (!isEpollfdUseful())
    {
        return false;
    }
    struct epoll_event ev;
    memset (&ev, 0, sizeof ev);
    ev.data.ptr  = co;
    ev.events = event;
    int ret = ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev);
    assert(ret == 0);
    return true;
}

bool Epoller::modifyEvent(Coroutine* co, int fd, int event)
{
    if (!isEpollfdUseful())
    {
        return false;
    }
    struct epoll_event ev;
    memset (&ev, 0, sizeof ev);
    ev.data.ptr  = co;
    ev.events = event;
    int ret = ::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev);
    assert(ret == 0);
    return true;
}

bool Epoller::getActiveEpollEvents(int timeoutMs, std::vector<Coroutine*>& activeCoroutines)
{
    if (!isEpollfdUseful())
    {
        return -1;
    }
    int actEvNum = ::epoll_wait(epollfd_, activeEpollEvents_.data(), static_cast<int>(activeEpollEvents_.size()), timeoutMs);
    int savedErrno = errno;
    if (actEvNum > 0)
    {
        for (int i = 0; i < actEvNum; ++i)
        {
            Coroutine* co = static_cast<Coroutine*>(activeEpollEvents_[i].data.ptr);
            activeCoroutines.emplace_back(co);
        }

        // 如果actEvNum等于activeEpollEvents_的大小，说明没有足够的空间容纳所有的epoll事件，需要扩容
        if (actEvNum == static_cast<int>(activeEpollEvents_.size()))
        {
            activeEpollEvents_.resize(activeEpollEvents_.size() * 2);
        }
    }
    return savedErrno;

}