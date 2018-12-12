#pragma once

#include "jitasm.h"
#include <udis86.h>

// Some parts contain the SteamComponent of CitizenFX Framework.
// Licensed under the following conditions:
/*
    Copyright(c) 2014 Bas Timmer / NTAuthority et al.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files(the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions :

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <immintrin.h>

#include <algorithm>
#include <codecvt>
#include <future>
#include <type_traits>
#include <unordered_map>

namespace hk
{
constexpr auto kExecutableAddress = 0x140000000;

namespace hooking_helpers
{
    void *AllocInHookSection(size_t size);

    class section_info
    {
      private:
        uintptr_t _begin;
        uintptr_t _end;

      public:
        decltype(auto) begin()
        {
            return _begin;
        }
        decltype(auto) end()
        {
            return _end;
        }

        section_info(uintptr_t begin, uintptr_t end)
            : _begin(begin)
            , _end(end)
        {
        }
    };

    void *AllocateFunctionStub(void *function);

    class AssemblyGen
    {
      private:
        void * _code = nullptr;
        size_t _size;

      public:
        inline AssemblyGen(jitasm::Frontend &frontend)
        {
            frontend.Assemble();

            void *code = nullptr;
            // We want our code to work with all allocation locations
            code = AllocInHookSection(frontend.GetCodeSize());

            if (!code) {
                OutputDebugStringA("Warning Allocating using 0. This could/will break the rel offset calls");
                code = VirtualAlloc(nullptr, frontend.GetCodeSize(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
            }

            memcpy(code, frontend.GetCode(), frontend.GetCodeSize());
            _size = frontend.GetCodeSize();

            _code = code;
        }

        inline ~AssemblyGen() = default;
        inline void *GetCode()
        {
            return _code;
        }

        inline size_t GetSize()
        {
            return _size;
        }
    };

    template <typename R, typename... Args> struct CallStub : jitasm::function<void, CallStub<R, Args...>> {
      private:
        void *m_target;

      public:
        CallStub(void *target)
            : m_target(target)
        {
        }

        void naked_main()
        {
            this->mov(this->rax, reinterpret_cast<uintptr_t>(m_target));
            this->call(this->rax);
        }
    };

    template <typename ValueType, typename AddressType> inline void put(AddressType address, ValueType value)
    {
        DWORD oldProtect;
        VirtualProtect((void *)address, sizeof(value), PAGE_EXECUTE_READWRITE, &oldProtect);

        memcpy((void *)address, &value, sizeof(value));

        VirtualProtect((void *)address, sizeof(value), oldProtect, &oldProtect);
    }
}; // namespace hooking_helpers

using hooking_helpers::put;

template <typename ValueType, typename AddressType> uintptr_t *detour_func(AddressType address, ValueType target)
{
    auto *code = reinterpret_cast<char *>(hooking_helpers::AllocateFunctionStub(target));

    ud_t ud;
    ud_init(&ud);

    ud_set_mode(&ud, 64);

    uint64_t k = address;
    ud_set_pc(&ud, k);
    ud_set_input_buffer(&ud, reinterpret_cast<uint8_t *>(address), INT64_MAX);

    auto opsize = ud_disassemble(&ud);
    while (opsize <= 12) {
        opsize += ud_disassemble(&ud);
    }

    opsize += 12;

    auto orig_code = reinterpret_cast<char *>(hooking_helpers::AllocInHookSection(opsize));

    opsize -= 12;

    memcpy(orig_code, (void *)address, opsize);
    auto code2                = orig_code + opsize;
    *(uint8_t *)code2         = 0x48;
    *(uint8_t *)(code2 + 1)   = 0xb8;
    *(uint64_t *)(code2 + 2)  = (uint64_t)(address + opsize);
    *(uint16_t *)(code2 + 10) = 0xE0FF;

    DWORD oldProtect;
    VirtualProtect((void *)address, 12, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy((void *)address, code, 12);

    VirtualProtect((void *)address, 12, oldProtect, &oldProtect);

    return (uintptr_t *)orig_code;
}

template <typename ValueType, typename AddressType> uintptr_t *detour_func_rdx(AddressType address, ValueType target)
{
    auto *code = reinterpret_cast<char *>(hooking_helpers::AllocateFunctionStub(target));

    ud_t ud;
    ud_init(&ud);

    ud_set_mode(&ud, 64);

    uint64_t k = address;
    ud_set_pc(&ud, k);
    ud_set_input_buffer(&ud, reinterpret_cast<uint8_t *>(address), INT64_MAX);

    auto opsize = ud_disassemble(&ud);
    while (opsize <= 12) {
        opsize += ud_disassemble(&ud);
    }

    opsize += 12;

    auto orig_code = reinterpret_cast<char *>(hooking_helpers::AllocInHookSection(opsize));

    opsize -= 12;

    memcpy(orig_code, (void *)address, opsize);
    auto code2                = orig_code + opsize;
    *(uint8_t *)code2         = 0x48;
    *(uint8_t *)(code2 + 1)   = 0xba;
    *(uint64_t *)(code2 + 2)  = (uint64_t)(address + opsize);
    *(uint16_t *)(code2 + 10) = 0xE2FF;

    DWORD oldProtect;
    VirtualProtect((void *)address, 12, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy((void *)address, code, 12);

    VirtualProtect((void *)address, 12, oldProtect, &oldProtect);

    return (uintptr_t *)orig_code;
}

template <typename ValueType, typename AddressType> uintptr_t *detour_func_rbx(AddressType address, ValueType target)
{
    auto *code = reinterpret_cast<char *>(hooking_helpers::AllocateFunctionStub(target));

    ud_t ud;
    ud_init(&ud);

    ud_set_mode(&ud, 64);

    uint64_t k = address;
    ud_set_pc(&ud, k);
    ud_set_input_buffer(&ud, reinterpret_cast<uint8_t *>(address), INT64_MAX);

    auto opsize = ud_disassemble(&ud);
    while (opsize <= 12) {
        opsize += ud_disassemble(&ud);
    }

    opsize += 12;

    auto orig_code = reinterpret_cast<char *>(hooking_helpers::AllocInHookSection(opsize));

    opsize -= 12;

    memcpy(orig_code, (void *)address, opsize);
    auto code2                = orig_code + opsize;
    *(uint8_t *)code2         = 0x48;
    *(uint8_t *)(code2 + 1)   = 0xbb;
    *(uint64_t *)(code2 + 2)  = (uint64_t)(address + opsize);
    *(uint16_t *)(code2 + 10) = 0xE3FF;

    DWORD oldProtect;
    VirtualProtect((void *)address, 12, PAGE_EXECUTE_READWRITE, &oldProtect);

    memcpy((void *)address, code, 12);

    VirtualProtect((void *)address, 12, oldProtect, &oldProtect);

    return (uintptr_t *)orig_code;
}

template <typename AddressType> inline void nop(AddressType address, size_t length)
{
    DWORD oldProtect;
    VirtualProtect((void *)address, length, PAGE_EXECUTE_READWRITE, &oldProtect);

    memset((void *)address, 0x90, length);

    VirtualProtect((void *)address, length, oldProtect, &oldProtect);
}

template <typename AddressType> inline void retn(AddressType address, uint16_t stackSize = 0)
{
    if (stackSize == 0) {
        hooking_helpers::put<uint8_t>(address, 0xC3);
    } else {
        hooking_helpers::put<uint8_t>(address, 0xC2);
        hooking_helpers::put<uint16_t>((uintptr_t)address + 1, stackSize);
    }
}

template <typename T, typename AT> inline void jump(AT address, T func)
{
    LPVOID funcStub = hooking_helpers::AllocateFunctionStub((void *)func);

    hooking_helpers::put<uint8_t>(address, 0xE9);
    hooking_helpers::put<int64_t>(static_cast<int64_t>((uintptr_t)address + 1),
                                  static_cast<int32_t>((intptr_t)funcStub - (intptr_t)address - 5));
}

template <typename T, typename AT> inline void call(AT address, T func)
{
    LPVOID funcStub = hooking_helpers::AllocateFunctionStub((void *)func);

    hooking_helpers::put<uint8_t>(address, 0xE8);
    hooking_helpers::put<int64_t>(static_cast<int64_t>((uintptr_t)address + 1),
                                  static_cast<int32_t>((intptr_t)funcStub - (intptr_t)address - 5));
}

template <typename T> inline T get_call(T address)
{
    intptr_t target = *(uintptr_t *)(address + 1);
    target += (address + 5);

    return (T)target;
}

template <typename TTarget, typename T> inline void set_call(TTarget *target, T address)
{
    *(T *)target = get_call(address);
}

template <typename R, typename... Args> class inject_jump
{
  private:
    R (*m_origAddress)(Args...);

    uintptr_t _address;

    std::unique_ptr<hooking_helpers::AssemblyGen> _assembly;

  public:
    inject_jump(uintptr_t address)
    {
        if (*(uint8_t *)address != 0xE9) {
            throw std::exception("not a jump");
        }
        _address = address;
    }

    void inject(R (*target)(Args...))
    {
        hooking_helpers::CallStub<R, Args...> stub(target);

        _assembly = std::make_unique<hooking_helpers::AssemblyGen>(stub);

        uintptr_t addressOffset = 1;

        // store original
        int64_t origAddress = *(int32_t *)(_address + addressOffset);
        origAddress += (4 + addressOffset);
        origAddress += _address;

        m_origAddress = (R(*)(Args...))origAddress;
#pragma warning(push)
#pragma warning(disable : 4311 4302)
        ud_t ud;
        ud_init(&ud);

        ud_set_mode(&ud, 64);

        intptr_t addressPtr = ((intptr_t)_assembly->GetCode() - (intptr_t)_address - (4 + addressOffset));
        auto address = static_cast<int32_t>((intptr_t)_assembly->GetCode() - (intptr_t)_address - (4 + addressOffset));

        if (addressPtr != address) {
            assert(false);
        } else {
            // Write a jump rax to the end instead of the call
            WORD data = 0xE0FF;
            memcpy((void *)((uintptr_t)_assembly->GetCode() + (uintptr_t)_assembly->GetSize() - 2), (void *)&data, 2);

            // Just replace a nop with a int 3 to debug the hook call assembly code
            // BYTE bData = 0xCC;
            // memcpy((void*)((uintptr_t)_assembly->GetCode()), (void*)&bData, 1);

            // Patch call opcode so its not a call to another DLL
            hooking_helpers::put<int32_t>(_address + addressOffset, address);
        }
#pragma warning(pop)
    }

    R call()
    {
        return ((R(*)())m_origAddress)();
    }

    R call(Args... args)
    {
        return m_origAddress(args...);
    }
};

template <typename R, typename... Args> class inject_call
{
  private:
    R (*m_origAddress)(Args...);

    uintptr_t _address;

    std::unique_ptr<hooking_helpers::AssemblyGen> _assembly;

  public:
    inject_call(uintptr_t address)
    {
#if _x64
        if (*(uint16_t *)address != 0x15FF && (*(uint8_t *)address != 0xE8)) {
            throw std::exception("not a call");
        }
#else
        if (*(uint8_t *)address != 0xE8) {
            throw std::exception("not a call");
        }
#endif
        _address = address;
    }

    void inject(R (*target)(Args...))
    {
        hooking_helpers::CallStub<R, Args...> stub(target);

        _assembly = std::make_unique<hooking_helpers::AssemblyGen>(stub);

        uintptr_t addressOffset = 1;

        if (*(uint8_t *)_address != 0xE8) {
            addressOffset = 2;
        }

        // store original
        int64_t origAddress = *(int32_t *)(_address + addressOffset);
        origAddress += (4 + addressOffset);
        origAddress += _address;

        m_origAddress = (R(*)(Args...))origAddress;
#pragma warning(push)
#pragma warning(disable : 4311 4302)
        ud_t ud;
        ud_init(&ud);

        ud_set_mode(&ud, 64);

        intptr_t addressPtr = ((intptr_t)_assembly->GetCode() - (intptr_t)_address - (4 + addressOffset));
        auto address = static_cast<int32_t>((intptr_t)_assembly->GetCode() - (intptr_t)_address - (4 + addressOffset));

        if (addressPtr != address) {
            assert(false);
        } else {
            // Write a jump rax to the end instead of the call
            WORD data = 0xE0FF;
            memcpy((void *)((uintptr_t)_assembly->GetCode() + (uintptr_t)_assembly->GetSize() - 2), (void *)&data, 2);

            // Just replace a nop with a int 3 to debug the hook call assembly code
            // BYTE bData = 0xCC;
            // memcpy((void*)((uintptr_t)_assembly->GetCode()), (void*)&bData, 1);

            // Patch call opcode so its not a call to another DLL
            if (*(uint16_t *)_address == 0x15FF) {
                uint16_t d = 0xE890;
                hooking_helpers::put<int16_t>(_address, d);
            }

            hooking_helpers::put<int32_t>(_address + addressOffset, address);
        }
#pragma warning(pop)
    }

    template <size_t _Args = sizeof...(Args)> typename std::enable_if<(_Args > 0), R>::type call()
    {
        return ((R(*)())m_origAddress)();
    }

    R call(Args... args)
    {
        return m_origAddress(args...);
    }

    uintptr_t GetAddress()
    {
        return (uintptr_t)m_origAddress;
    }
};

inline bool set_import(const std::string &name, uintptr_t func)
{
    bool result    = false;
    auto dosHeader = (PIMAGE_DOS_HEADER)(kExecutableAddress);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        throw std::runtime_error("Invalid DOS Signature");
    }

    auto header = (PIMAGE_NT_HEADERS)((kExecutableAddress + (dosHeader->e_lfanew * sizeof(char))));
    if (header->Signature != IMAGE_NT_SIGNATURE) {
        throw std::runtime_error("Invalid NT Signature");
    }

    // BuildImportTable
    PIMAGE_DATA_DIRECTORY directory = &header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

    if (directory->Size > 0) {
        auto importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(header->OptionalHeader.ImageBase + directory->VirtualAddress);
        for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++) {
            static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
            auto name2  = (LPCSTR)(header->OptionalHeader.ImageBase + importDesc->Name);
            auto sname  = converter.from_bytes(name2);
            auto csname = sname.c_str();

            HMODULE handle = LoadLibraryW(csname);

            if (handle == nullptr) {
                SetLastError(ERROR_MOD_NOT_FOUND);
                break;
            }

            auto *thunkRef = (uintptr_t *)(header->OptionalHeader.ImageBase + importDesc->OriginalFirstThunk);
            auto *funcRef  = (FARPROC *)(header->OptionalHeader.ImageBase + importDesc->FirstThunk);

            if (!importDesc->OriginalFirstThunk) // no hint table
            {
                thunkRef = (uintptr_t *)(header->OptionalHeader.ImageBase + importDesc->FirstThunk);
            }

            for (; *thunkRef, *funcRef; thunkRef++, (void)funcRef++) {
                if (!IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
                    std::string import =
                        (LPCSTR) & ((PIMAGE_IMPORT_BY_NAME)(header->OptionalHeader.ImageBase + (*thunkRef)))->Name;

                    if (import == name) {
                        DWORD oldProtect;
                        VirtualProtect((void *)funcRef, sizeof(FARPROC), PAGE_EXECUTE_READWRITE, &oldProtect);

                        *funcRef = (FARPROC)func;

                        VirtualProtect((void *)funcRef, sizeof(FARPROC), oldProtect, &oldProtect);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

template <typename R> static R __thiscall func_call(uint64_t addr)
{
    return ((R(*)())(addr))();
}

template <typename R, typename... Args> struct func_call_member_helper {
    using func_t = R (func_call_member_helper::*)(Args...) const;

    R operator()(func_t func_ptr, Args... args) const
    {
        return (this->*func_ptr)(args...);
    }
};

template <typename R, class T, typename... Args> R func_call_member(uint64_t addr, T _this, Args... args)
{
    const func_call_member_helper<R, Args...> *helper =
        reinterpret_cast<const func_call_member_helper<R, Args...> *>(_this);
    auto           func_ptr = *(typename std::remove_pointer_t<decltype(helper)>::func_t *)(&addr);
    return helper->operator()(func_ptr, args...);
}

template <typename R, class T, typename... Args>
inline std::enable_if_t<!std::is_pointer_v<T> || !std::is_class_v<std::remove_pointer_t<T>>, R>
func_call(uint64_t addr, T _this, Args... args)
{
    return ((R(*)(T, Args...))(addr))(_this, args...);
}

template <typename R, class T, typename... Args>
inline std::enable_if_t<std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>>, R>
func_call(uint64_t addr, T _this, Args... args)
{
    return func_call_member<R, T, Args...>(addr, _this, args...);
}

template <typename T> struct native_member_func_helper;
template <typename _Ret, typename... Args> struct native_member_func_helper<_Ret(Args...)> {
    uintptr_t addr;
    constexpr native_member_func_helper(uintptr_t addr)
        : addr{addr}
    {
    }
    template <typename Ty> inline auto operator()(Ty a, Args... args) const
    {
        return ((_Ret(*)(Ty, Args...))(addr))(a, args...);
    }
};
}; // namespace hk