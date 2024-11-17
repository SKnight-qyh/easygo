#pragma once

#include <iostream>
#include <string>

class Timestamp
{

public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch); //禁止隐式转换
    static Timestamp now();
    std::string toString() const;   // 只读方法 const 关键字表示这个成员函数不会修改对象的任何成员变量

private:
    int64_t microSecondsSinceEpoch_;
};