#include <Windows.h>
#include <algorithm>
#include <fstream>

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

#if 0
static std::map<std::string, int32_t> _spawn_factories;
static std::map<std::string, int32_t> _registered_events;
static std::map<std::string, int32_t> _called_events;
#endif

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
            Graphics::Get()->Init(device);
            Input::Get()->Draw();

            flip.call(device);
        });

        // dump some strings
#if 0
        // CSpawnSystem tags
        static hk::inject_jump<int32_t, uintptr_t, const char *, int64_t *, uint32_t *> parse_tags(0x140A2AA20);
        parse_tags.inject([](uintptr_t spawn_factory, const char *tagstr, int64_t *tags, uint32_t *numinvalidtags) {
			_spawn_factories[tagstr]++;
            std::ofstream file("C:/users/aaron/desktop/factories.txt");
            for (auto &e : _spawn_factories) {
                file << e.first << std::endl;
            }
            file.close();
            return parse_tags.call(spawn_factory, tagstr, tags, numinvalidtags);
        });

        // Event handler strings
        static hk::inject_jump<int64_t, const char *, uintptr_t, int32_t> get_events(0x140087B80);
        get_events.inject([](const char *event_name, uintptr_t events, int32_t max) {
            _called_events[event_name]++;
            std::ofstream file("C:/users/aaron/desktop/called_events.txt");
            for (auto &e : _called_events) {
                file << e.first << std::endl;
            }
            file.close();
            return get_events.call(event_name, events, max);
        });

        // register events
        static hk::inject_jump<void, uintptr_t, const char *, int64_t, bool> register_event(0x140089870);
        register_event.inject([](uintptr_t a1, const char *event_name, int64_t a3, bool a4) {
            _registered_events[event_name]++;
            std::ofstream file("C:/users/aaron/desktop/registered_events.txt");
            for (auto &e : _registered_events) {
                file << e.first << std::endl;
            }
            file.close();
            return register_event.call(a1, event_name, a3, a4);
        });
#endif
    }

    return TRUE;
}
