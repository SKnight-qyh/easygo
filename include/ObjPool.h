#pragma once
#include "noncopyable.h"
#include "MemPool.h"

#include <type_traits>
namespace easygo
{
template<class T>
class ObjPool : public noncopyable
{
public:
    ObjPool() {};
    ~ObjPool() {};
    template<typename... Args>
    inline T* newObj(Args&&... args);
    inline void deleteObj(void* obj);

private:
    template<typename... Args>
    inline T* newAux(std::true_type, Args&&... args);

    template<typename... Args>
    inline T* newAux(std::false_type, Args&&... args);

    inline void deleteAux(std::true_type, void* obj);

    inline void deleteAux(std::false_type, void* obj);

    MemPool<sizeof(T)> memPool_;

};//end ObjPoll


// Template Metaprogramming to determine whether T is trivially constructible
// 如果 T 的构造函数是平凡的（即没有用户定义的构造函数，或者构造函数是默认的、无操作的），则返回 true；否则返回 false

template<class T>
template<typename... Args>
inline T* ObjPool<T>::newObj(Args&&... args)
{
    return new_aux(std::integral_constant<bool, std::is_trivially_constructible<T>::value>(), std::forward<Args>(args)...);
}

template<class T>
template<typename... Args>
inline T* ObjPool<T>::newAux(std::true_type, Args&&... args)
{
    return static_cast<T*>(memPool_.AllocAMemBlock());
}

template<class T>
template<typename... Args>
inline T* ObjPool<T>::newAux(std::false_type, Args&&... args)
{
    void* newPos = memPool_.AllocAMemBlock();
    return new(newPos) T(std::forward<Args>(args)...);
}


template<class T>
inline void ObjPool<T>::deleteObj(void* obj)
{
    if (nullptr == obj)
    {
        return;
    }
    deleteAux(std::integral_constant<bool, std::is_trivially_destructible<T>::value>(), obj);
}

template<class T>
inline void ObjPool<T>::deleteAux(std::true_type, void* obj)
{
    memPool_.freeMemBlock(obj);

}

template<class T>
inline void ObjPool<T>::deleteAux(std::false_type, void* obj)
{
    static_cast<T*>(obj)->~T();
    memPool_.freeMemBlock(obj);

}

}//end easygo