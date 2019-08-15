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
        return **(CManagerBase**)0x142CD4B30;
    }

    void LoseFocus()
    {
        hk::func_call<void>(0x147C63280, this);
    }

    void GainFocus()
    {
        hk::func_call<void>(0x147C630D0, this);
    }
};
}; // namespace jc::NInput
#pragma pack(pop)
