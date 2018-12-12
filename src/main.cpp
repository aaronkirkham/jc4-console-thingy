#include <Windows.h>
#include <fstream>

#include "hooking/hooking.h"
#include "xinput9_1_0.h"

#include "graphics.h"
#include "util.h"

#include "game/debug_renderer_impl.h"
#include "game/device.h"
#include "game/game_object.h"
#include "game/player_manager.h"
#include "game/spawn_system.h"

#include "commands/command_handler.h"

#include "commands/event_command.h"
#include "commands/spawn_command.h"

#if 0
// 0x140A11460
uintptr_t *orig = nullptr;
void *     CreateFactory(uintptr_t spawn_factory, const char *tagstr, unsigned int flags, char source)
{
    std::stringstream str;
    str << tagstr << " " << source << std::endl;

    std::ofstream file("C:/users/aaron/desktop/createfactory.txt", std::ios::app);
    file << str.str();
    file.close();

    return ((decltype(CreateFactory) *)(orig))(spawn_factory, tagstr, flags, source);
}
#endif

/*
static auto render_engine  = *(void **)0x142A1BCA8;
static auto debug_renderer = *(jc::DebugRendererImpl **)((char *)render_engine + 0x2BB0);
*/

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	// exit if not jc4
	if (!GetModuleHandle("JustCause4.exe")) {
        return FALSE;
	}

    if (fdwReason == DLL_PROCESS_ATTACH) {
		// allocate a section for hooking things
        VirtualAlloc((LPVOID)0x0000000160000000, 0x6000000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

        char buffer[MAX_PATH] = {0};
        GetSystemDirectoryA(buffer, MAX_PATH);
        strcat_s(buffer, MAX_PATH, "\\xinput9_1_0.dll");

        const auto module = LoadLibraryA(buffer);
        if (module) {
            XInputGetState_ = (XInputGetState_t)GetProcAddress(module, "XInputGetState");
            XInputSetState_ = (XInputSetState_t)GetProcAddress(module, "XInputSetState");
        }

        CommandHandler::Get()->Register("event", EventCommand::Handler);
        CommandHandler::Get()->Register("spawn", SpawnCommand::Handler);

        static hk::inject_jump<LRESULT, HWND, UINT, WPARAM, LPARAM> wndproc(0x140AF6470);
        wndproc.inject([](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (CommandHandler::Get()->WndProc(uMsg, wParam, lParam)) {
                return 0;
            }

            return wndproc.call(hwnd, uMsg, wParam, lParam);
        });

        static hk::inject_jump<void, jc::HDevice_t *> flip(0x140DF4E90);
        flip.inject([](jc::HDevice_t *device) -> void {
            Graphics::Get()->Init(device);

            // auto str = util::string_format("0x%p", device);
            // Graphics::Get()->DrawString(str, {10, 10}, 22, 0xFFFFFFFF);

            CommandHandler::Get()->Draw();

            if (GetAsyncKeyState(VK_F10) & 0x1) {
                TerminateProcess(GetCurrentProcess(), -1);
            }

            flip.call(device);
        });

        // orig = hk::detour_func(0x140A11460, CreateFactory);

#if 0
        static hk::inject_jump<int32_t, uintptr_t, const char *, uint32_t, char *> parse_tags(0x140A2AA20);
        parse_tags.inject([](uintptr_t spawn_factory, const char *tagstr, unsigned int flags, char *source) {
            std::stringstream str;
            str << tagstr << " " << source << std::endl;

            std::ofstream file("C:/users/aaron/desktop/spawn_factory.txt", std::ios::app);
            file << str.str();
            file.close();
            return parse_tags.call(spawn_factory, tagstr, flags, source);
        });
#endif
    }

    return TRUE;
}
