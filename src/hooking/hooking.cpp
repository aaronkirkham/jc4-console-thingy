#include <Windows.h>
#include <cstdint>

#include "hooking.h"

constexpr uintptr_t kHookSectionOffset = 0x20000000;

void* hk::hooking_helpers::AllocInHookSection(size_t size)
{
    static bool firstCall = true;

    auto addr = kExecutableAddress + kHookSectionOffset;
    if (firstCall)
    {
        DWORD oldProtect;
        VirtualProtect((LPVOID)(addr), size, PAGE_READWRITE, &oldProtect);
        *(uint32_t*)(addr) = sizeof(uint32_t);
        firstCall = false;
    }

    auto code = (LPVOID)(addr + *(uint32_t*)(addr));
    DWORD oldProtect;
    VirtualProtect(code, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    *(uint32_t*)(addr) += static_cast<uint32_t>(size);
    return code;
}

void* hk::hooking_helpers::AllocateFunctionStub(void* function)
{
    char *code = reinterpret_cast<char*>(AllocInHookSection(20));
    *(uint8_t*)code = 0x48;
    *(uint8_t*)(code + 1) = 0xb8;
    *(uint64_t*)(code + 2) = (uint64_t)function;
    *(uint16_t*)(code + 10) = 0xE0FF;
    *(uint64_t*)(code + 12) = 0xCCCCCCCCCCCCCCCC;
    return code;
}
