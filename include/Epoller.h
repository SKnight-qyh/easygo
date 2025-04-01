#pragma once

#include "noncopyable.h"
#include "Coroutine.h"

#include <sys/epoll.h>
#include <vector>

namespace netco
{
class Coroutine;

// 增删改查事件
class Epoller : public noncopyable
{
public:
    Epoller();
    ~Epoller();

    bool init();

    bool addEvent(Coroutine* co, int fd, int event);

    bool removeEvent(Coroutine* co, int fd, int event);

    bool modifyEvent(Coroutine* co, int fd, int event);

    bool getActiveEpollEvents(int timeoutMs, std::vector<Coroutine*>& activeCos);
private:
    int epollfd_;

    inline bool isEpollfdUseful() const 
    { return epollfd_ >= 0; }

    std::vector<struct epoll_event> activeEpollEvents_;

};//end Epoller
}// end netco