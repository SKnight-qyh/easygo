#pragma once
namespace easygo {
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable(const noncopyable&&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
    noncopyable() = default;
    ~noncopyable() = default;
};

}