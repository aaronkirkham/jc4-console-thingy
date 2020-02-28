#pragma once

#include "hooking/hooking.h"

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
        m_allocator = hk::func_call<void*>(0x140F236F0);
    }

    ~TArray()
    {
        if (m_allocator && m_begin) {
            hk::func_call<void>(0x140838070, m_allocator, m_begin, sizeof(T));
        }
    }
};
}; // namespace jc
