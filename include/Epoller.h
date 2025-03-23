#pragma once

#include "noncopyable.h"
namespace netco
{
class Processer;

class Epoller : public noncopyable
{
public:
    Epoller();
    ~Epoller();
    bool addEvent(Processer* pro, int fd, int event);

};//end Epoller
}// end netco