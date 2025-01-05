#include "../include/Scheduler.h"

using namespace netco;

Scheduler* Scheduler::_pScheduler = nullptr;
std::mutex Scheduler::_schMutex;

Scheduler::Scheduler()
    : _proSeletor(_pros)
{ }