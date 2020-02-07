#pragma once

namespace jc
{
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
        return hk::func_call<pointer>(0x140A51DC0, (n * sizeof(value_type))); // operator new()
    }

    void deallocate(pointer p, size_type n)
    {
        hk::func_call<void>(0x141AF8E1C, p); // operator delete[]
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
