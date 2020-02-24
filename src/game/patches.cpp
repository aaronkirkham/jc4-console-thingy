#include "patches.h"
#include "character.h"

#include "clock.h"

#include "graphics.h"
#include "input.h"

#include "hooking/hooking.h"

namespace jc
{
struct CFiringModule {
    char _pad[0x310];
    struct {
        char                      _pad[0x5D0];
        std::weak_ptr<CCharacter> m_user;
    } * m_weapon;
    char     _pad2[0x24C];
    uint32_t m_ammoType; // 564
};

static uintptr_t *CFiringModule__ConsumeAmmo_orig = nullptr;
static void       CFiringModule__ConsumeAmmo(CFiringModule *_this, uintptr_t weapon_data, int64_t ammo)
{
    const uint32_t ammo_type = _this->m_ammoType;

    // if unlimited ammo is enabled, set the ammo type to unlimited (this will have an effect on vehicles).
    if (_this->m_weapon) {
        auto character = _this->m_weapon->m_user.lock();
        if (character && character->m_unlimitedAmmo) {
            _this->m_ammoType = 13; // AMMO_TYPE_UNLIMITED
        }
    }

    ((decltype(CFiringModule__ConsumeAmmo) *)(CFiringModule__ConsumeAmmo_orig))(_this, weapon_data, ammo);

    // restore old ammo type
    _this->m_ammoType = ammo_type;
}

void InitPatchesAndHooks()
{
    // allocate a section for hooking things
    VirtualAlloc((LPVOID)0x0000000160000000, 0x6000000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

#ifdef DEBUG
    bool quick_start = true;
#else
    bool quick_start = (strstr(GetCommandLine(), "-quickstart") != nullptr);
#endif

    // enable quick start
    if (quick_start) {
        // quick start
        hk::put<bool>(0x142CB8F40, true);

        // IsIntroSequenceComplete always returns true
        hk::put<uint32_t>(0x140E935B0, 0x90C301B0);

        // IsIntroMovieComplete always returns true
        hk::put<uint32_t>(0x140E93530, 0x90C301B0);
    }

    // WndProc
    static hk::inject_jump<LRESULT, HWND, UINT, WPARAM, LPARAM> WndProc(0x140C7FB50);
    WndProc.inject([](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        auto game_state   = *(uint32_t *)0x142CB8F24;
        auto suspend_game = *(bool *)0x142CBDAF0;
        auto clock        = &jc::Base::CClock::instance();

        if (game_state == 3 && clock) {
            if (!suspend_game && !clock->m_paused) {
                if (Input::Get()->WndProc(uMsg, wParam, lParam)) {
                    return 0;
                }
            }
            // disable input if the game is suspended or the clock is paused.
            // This fixes issues with window messages not being handled as we stole the message
            else if (Input::Get()->IsInputEnabled()) {
                Input::Get()->EnableInput(false);
            }
        }

        return WndProc.call(hwnd, uMsg, wParam, lParam);
    });

    // Graphics::Flip
    static hk::inject_jump<void, jc::HDevice_t *> FlipThread(0x140FA2C70);
    FlipThread.inject([](jc::HDevice_t *device) -> void {
        Graphics::Get()->BeginDraw(device);

        // draw input
        Input::Get()->Draw();

        Graphics::Get()->EndDraw();

        FlipThread.call(device);
    });

    // override ConsumeAmmo to fix unlimited ammo not being applied to vehicles
    CFiringModule__ConsumeAmmo_orig = hk::detour_func(0x140728840, CFiringModule__ConsumeAmmo);
}
}; // namespace jc
