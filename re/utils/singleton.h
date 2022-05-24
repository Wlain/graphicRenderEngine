//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_SINGLETON_H
#define SIMPLERENDERENGINE_SINGLETON_H
#include <type_traits>

template <typename T>
class Singleton
{
protected:
    Singleton() = default;

public:
    static T& getInstance() noexcept(std::is_nothrow_constructible<T>::value)
    {
        static T instance;
        return instance;
    }
    virtual ~Singleton() noexcept = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};
#endif //SIMPLERENDERENGINE_SINGLETON_H
