#pragma once

#include "hooking/hooking.h"

#pragma pack(push, 1)
namespace jc::NInput
{
class CManagerBase
{
  public:
    static CManagerBase& instance()
    {
        return **(CManagerBase**)0x142CE1AF0;
    }

    void LoseFocus()
    {
        hk::func_call<void>(0x140FC8560, this);
    }

    void GainFocus()
    {
        hk::func_call<void>(0x140FC84B0, this);
    }
};
}; // namespace jc::NInput
#pragma pack(pop)