#ifndef __SYLAR_SINGLETON_H__
#define __SYLAR_SINGLETON_H__

#include <memory>

namespace sylar
{

    template <class T, class X = void, int N = 0>
    class Singleton
    {
    public:
        static T* GetInstance(){
            static T V;
            return &V;
        }
    };


    template <class T, class X = void, int N = 0>
    class SingletonPtr{
    public:
        static std::shared_ptr<T> GetInstance(){
            static std::shared_ptr<T> V(new T);
        }
    };

} // namespace sylar

#endif