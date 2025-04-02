

#include "../include/Timestamp.h"

using namespace easygo;

Timestamp::Timestamp()
    :microSecondsSinceEpoch_(0) {}

Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    :microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {}

//返回当前时间
Timestamp Timestamp::now()
{
    return Timestamp(time(nullptr));  
}
std::string Timestamp::toString() const   // 只读方法 const 关键字表示这个成员函数不会修改对象的任何成员变量
{
    char buf[32] = {0};
    tm *tm_time = localtime(&microSecondsSinceEpoch_);
    snprintf(buf, 32, "%4d/%02d/%02d %02d:%02d:%02d",
        tm_time->tm_year + 1990,
        tm_time->tm_mon + 1,
        tm_time->tm_mday,
        tm_time->tm_hour,
        tm_time->tm_min,
        tm_time->tm_sec);
    
    return buf;

}

// #include <iostream>
// int main(){
//     std::cout << Timestamp::now().toString() << std::endl;
//     std::cout << "hello" << std::endl;
//     return 0;
// }