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
        return **(CManagerBase**)0x142C7DAF0;
    }

    void LoseFocus()
    {
        hk::func_call<void>(0x140F7D3E0, this);
    }

    void GainFocus()
    {
        hk::func_call<void>(0x140F7D330, this);
    }
};
}; // namespace jc::NInput
#pragma pack(pop)
