#ifndef ENGINE_STORAGE_H
#define ENGINE_STORAGE_H

namespace helpers::storage
{
    class ProxyStorage
    {
    public:
        template<class T>
        static T *allocate()
        {
            return new T;
        }
        template<class T>
        static void remove(T *p)
        {
            if (p != nullptr)
            {
                delete p;
            }
        }
    };

    class ProxyStorageDeleter
    {
    public:
        template<class T>
        void operator()(T *p)
        {
            ProxyStorage::remove(p);
        }
    };
}


#endif //ENGINE_STORAGE_H
