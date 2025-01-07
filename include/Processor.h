#pragma once
#include "noncopyable.h"


#include <sys/types.h>
#include <thread>
#include <functional>

namespace netco 
{
enum ProStatus 
{
    PRO_RUNNING = 0,
    PRO_STOPPING,
    PRO_STOPPED
};

class Processor : public noncopyable
{
public:
    Processor(int id);
    ~Processor();
    size_t getCoCnt() const;
    void stop();
    void join();
    void loop();
    // run a new coroutine in this processor
    void goNewCo(std::function<void()>&& func, int stackSize);
    void goNewCo(std::function<void()>& func, int stackSize);
    
private:
    std::thread* _pLoop;
    int _status;    // 
    int _id;
    int _coCnt;
};

}