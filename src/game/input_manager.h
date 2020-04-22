#pragma once

#include <meow_hook/util.h>

#pragma pack(push, 1)
namespace jc::NInput
{
class CManagerBase
{
  public:
    static CManagerBase& instance()
    {
        return **(CManagerBase**)GetAddress(INST_INPUT_MANAGER);
    }

    void LoseFocus()
    {
        meow_hook::func_call<void>(GetAddress(INPUT_LOST_FOCUS), this);
    }

    void GainFocus()
    {
        meow_hook::func_call<void>(GetAddress(INPUT_GAIN_FOCUS), this);
    }
};
}; // namespace jc::NInput
#pragma pack(pop)
