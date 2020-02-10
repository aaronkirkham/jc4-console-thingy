#pragma once

#include "hooking/hooking.h"

namespace jc
{
inline void* _alloc(size_t size)
{
    return hk::func_call<void*>(0x140A51DC0, size);
}

inline void _free(void* p)
{
    hk::func_call<void>(0x141AF8E1C, p);
}

template <typename T> class allocator
{
  public:
    typedef T        value_type;
    typedef size_t   size_type;
    typedef T*       pointer;
    typedef const T* const_pointer;

    allocator()  = default;
    ~allocator() = default;

    pointer allocate(size_type n, const void* hint = 0)
    {
        return (pointer)jc::_alloc(n * sizeof(value_type));
    }

    void deallocate(pointer p, size_type n)
    {
        jc::_free(p);
    }

    inline bool operator==(allocator const&)
    {
        return true;
    }

    inline bool operator!=(allocator const& a)
    {
        return !operator==(a);
    }
};
}; // namespace jc
