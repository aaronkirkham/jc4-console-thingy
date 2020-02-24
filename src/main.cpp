#include <Windows.h>

#include "hooking/hooking.h"
#include "xinput9_1_0.h"

#include "input.h"

#include "game/game_world.h"
#include "game/player_manager.h"
#include "game/spawn_system.h"

#include "game/patches.h"

#include "commands/event.h"
#include "commands/spawn.h"
#include "commands/teleport.h"
#include "commands/world.h"

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
        if (*(uint32_t *)0x141E7EE40 != 0x6c617641) {
#ifdef DEBUG
            MessageBox(nullptr, "Wrong version.", nullptr, MB_ICONERROR | MB_OK);
#endif
            return TRUE;
        }

        // init
        jc::InitPatchesAndHooks();

        // register commands
        Input::Get()->RegisterCommand(std::make_unique<EventCommand>());
        Input::Get()->RegisterCommand(std::make_unique<SpawnCommand>());
        Input::Get()->RegisterCommand(std::make_unique<WorldCommand>());
        Input::Get()->RegisterCommand(std::make_unique<TeleportCommand>());
#ifdef DEBUG
        Input::Get()->RegisterCommand("exit",
                                      [](const std::string &arguments) { TerminateProcess(GetCurrentProcess(), -1); });
#endif
    }

    return TRUE;
}
