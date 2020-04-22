#include <Windows.h>
#include <filesystem>
#include <mutex>

#include "xinput9_1_0.h"

#include "input.h"

#include "game/game_world.h"
#include "game/player_manager.h"
#include "game/spawn_system.h"

#include "addresses.h"
#include "patches.h"

#include "commands/event.h"
#include "commands/skin.h"
#include "commands/spawn.h"
#include "commands/teleport.h"
#include "commands/world.h"

#include <meow_hook/memory.h>
#include <meow_hook/pattern_search.h>

static HMODULE g_original_module = nullptr;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    // exit if not jc4
    if (!GetModuleHandle("JustCause4.exe") && !GetModuleHandle("JustCause4GameCL.exe")) {
        return FALSE;
    }

    switch (fdwReason) {
        case DLL_PROCESS_ATTACH: {
            char buffer[MAX_PATH] = {0};
            GetSystemDirectoryA(buffer, MAX_PATH);
            strcat_s(buffer, MAX_PATH, "\\xinput9_1_0.dll");

            g_original_module = LoadLibraryA(buffer);
            if (g_original_module) {
                XInputGetState_ = (XInputGetState_t)GetProcAddress(g_original_module, "XInputGetState");
                XInputSetState_ = (XInputSetState_t)GetProcAddress(g_original_module, "XInputSetState");
            }

            // just checking the module handle is probably enough, but w/e.
            bool is_steam = !strstr(GetCommandLine(), "-epicusername") && GetModuleHandle("steam_api64.dll");
            jc::InitAddresses(is_steam);

            // something is wrong. sanity check failed. probably the game updated and offsets are wrong.
            if (!jc::InitPatchesAndHooks()) {
                return TRUE;
            }

            // register commands
            Input::Get()->RegisterCommand(std::make_unique<EventCommand>());
            Input::Get()->RegisterCommand(std::make_unique<SpawnCommand>());
            Input::Get()->RegisterCommand(std::make_unique<WorldCommand>());
            Input::Get()->RegisterCommand(std::make_unique<SkinCommand>());
            Input::Get()->RegisterCommand(std::make_unique<TeleportCommand>());
#ifdef DEBUG
            Input::Get()->RegisterCommand(
                "exit", [](const std::string &arguments) { TerminateProcess(GetCurrentProcess(), -1); });
#endif
            break;
        }

        case DLL_PROCESS_DETACH: {
            if (g_original_module) {
                FreeLibrary(g_original_module);
            }

            break;
        }
    }

    return TRUE;
}
