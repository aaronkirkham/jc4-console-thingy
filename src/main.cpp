#include <Windows.h>

#include "hooking/hooking.h"
#include "xinput9_1_0.h"

#include "graphics.h"
#include "input.h"
#include "util.h"

#include "game/debug_renderer_impl.h"
#include "game/device.h"
#include "game/game_object.h"
#include "game/player_manager.h"
#include "game/spawn_system.h"

#include "commands/event_command.h"
#include "commands/spawn_command.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    // exit if not jc4
    if (!GetModuleHandle("JustCause4.exe")) {
        return FALSE;
    }

    if (fdwReason == DLL_PROCESS_ATTACH) {
        char buffer[MAX_PATH] = {0};
        GetSystemDirectoryA(buffer, MAX_PATH);
        strcat_s(buffer, MAX_PATH, "\\xinput9_1_0.dll");

        const auto module = LoadLibraryA(buffer);
        if (module) {
            XInputGetState_ = (XInputGetState_t)GetProcAddress(module, "XInputGetState");
            XInputSetState_ = (XInputSetState_t)GetProcAddress(module, "XInputSetState");
        }

        // if we are running on the wrong version, don't continue
        if (*(uint32_t *)0x141C78F48 != 0x6c617641) {
#ifdef DEBUG
            MessageBox(nullptr, "Wrong version.", nullptr, MB_ICONERROR | MB_OK);
#endif
            return TRUE;
        }

        // allocate a section for hooking things
        VirtualAlloc((LPVOID)0x0000000160000000, 0x6000000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        Input::Get()->RegisterCommand(std::make_unique<EventCommand>());
        Input::Get()->RegisterCommand(std::make_unique<SpawnCommand>());
#ifdef DEBUG
        Input::Get()->RegisterCommand("exit",
                                      [](const std::string &arguments) { TerminateProcess(GetCurrentProcess(), -1); });
#endif

#ifdef DEBUG
        bool quick_start = true;
#else
        bool quick_start = (strstr(GetCommandLine(), "-quickstart") != nullptr);
#endif

        // enable quick start
        if (quick_start) {
            // quick start
            *(bool *)0x142A5F9EC = true;

            // IsIntroSequenceComplete always returns true
            hk::put<uint32_t>(0x140CBAB00, 0x90C301B0);

            // IsIntroMovieComplete always returns true
            hk::put<uint32_t>(0x140CBAA80, 0x90C301B0);
        }

        static hk::inject_jump<LRESULT, HWND, UINT, WPARAM, LPARAM> wndproc(0x140AF6470);
        wndproc.inject([](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
            auto game_state = *(uint32_t *)0x142A5F9C4;
            if (game_state == 3 && Input::Get()->WndProc(uMsg, wParam, lParam)) {
                return 0;
            }

            return wndproc.call(hwnd, uMsg, wParam, lParam);
        });

        static hk::inject_jump<void, jc::HDevice_t *> flip(0x140DF4E90);
        flip.inject([](jc::HDevice_t *device) -> void {
            Graphics::Get()->BeginDraw(device);

            // draw input
            Input::Get()->Draw();

            Graphics::Get()->EndDraw();

            flip.call(device);
        });
    }

    return TRUE;
}
