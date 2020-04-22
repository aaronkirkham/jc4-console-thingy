#pragma once

namespace jc
{
template <typename T> class TArray
{
  public:
    T*    m_begin;
    T*    m_end;
    T*    m_last;
    void* m_allocator;

    TArray()
    {
        m_begin     = nullptr;
        m_end       = nullptr;
        m_last      = nullptr;
        m_allocator = meow_hook::func_call<void*>(GetAddress(GET_DEFAULT_PLATFORM_ALLOCATOR));
    }

    ~TArray()
    {
        if (m_allocator && m_begin) {
            meow_hook::func_call<void>(GetAddress(PLATFORM_ALLOCATOR_FREE), m_allocator, m_begin, sizeof(T));
        }
    }
};
}; // namespace jc
