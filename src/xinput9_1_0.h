#pragma once

namespace
{
using XInputGetState_t = DWORD (*)(DWORD, uintptr_t);
using XInputSetState_t = DWORD (*)(DWORD, uintptr_t);

static XInputGetState_t XInputGetState_;
static XInputSetState_t XInputSetState_;

__declspec(dllexport) DWORD XInputGetState(DWORD dwUserIndex, uintptr_t pVibration)
{
    if (XInputGetState_) {
        return XInputGetState_(dwUserIndex, pVibration);
    }

    return E_FAIL;
}

__declspec(dllexport) DWORD XInputSetState(DWORD dwUserIndex, uintptr_t pVibration)
{
    if (XInputSetState_) {
        return XInputSetState_(dwUserIndex, pVibration);
    }

    return E_FAIL;
}
}; // namespace
