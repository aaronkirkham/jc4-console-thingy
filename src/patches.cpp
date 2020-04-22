#include "patches.h"
#include "addresses.h"

#include "game/character.h"
#include "game/clock.h"
#include "game/player_manager.h"
#include "game/skin_change_req_handler.h"

#include "graphics.h"
#include "input.h"

#include <meow_hook/detour.h>
#include <meow_hook/memory.h>

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

LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
void    FiringModuleConsumeAmmo(CFiringModule *, uintptr_t, int64_t);
void    PlayerManagerUpdate(CPlayerManager *, float);

static decltype(WndProc) *                pfn_WndProc     = nullptr;
static decltype(FiringModuleConsumeAmmo) *pfn_ConsumeAmmo = nullptr;
static decltype(PlayerManagerUpdate) *    pfn_Update      = nullptr;

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    const auto &input = Input::Get();

    const uint32_t game_state   = *(uint32_t *)GetAddress(VAR_GAME_STATE);
    const bool     suspend_game = *(bool *)GetAddress(VAR_SUSPEND_GAME);
    auto           clock        = &jc::Base::CClock::instance();

    if (game_state == 3 && clock) {
        if (!suspend_game && !clock->m_paused) {
            if (input->FeedEvent(msg, wParam, lParam)) {
                return 0;
            }
        }
        // disable input if the game is suspended or the clock is paused.
        // This fixes issues with window messages not being handled as we stole the message
        else if (input->IsInputEnabled()) {
            input->EnableInput(false);
        }
    }

    return pfn_WndProc(hwnd, msg, wParam, lParam);
}

static void FiringModuleConsumeAmmo(CFiringModule *_this, uintptr_t weapon_data, int64_t ammo)
{
    const uint32_t ammo_type = _this->m_ammoType;

    // if unlimited ammo is enabled, set the ammo type to unlimited (this will have an effect on vehicles).
    if (_this->m_weapon) {
        auto character = _this->m_weapon->m_user.lock();
        if (character && character->m_unlimitedAmmo) {
            _this->m_ammoType = 13; // AMMO_TYPE_UNLIMITED
        }
    }

    pfn_ConsumeAmmo(_this, weapon_data, ammo);

    // restore old ammo type
    _this->m_ammoType = ammo_type;
}

static void PlayerManagerUpdate(CPlayerManager *_this, float dt)
{
    pfn_Update(_this, dt);

    jc::SkinChangeRequestHandler::Get()->Update();

    // if the local player is invulnerable, we want the same behaviour applied to their current vehicle
    {
        static CGameObject *invul_vehicle   = nullptr;
        static auto         SetInvulnerable = [](CGameObject *game_object, bool invulnerable) {
            // see: 0x1485ce1e0
            *(uint8_t *)((char *)game_object + 0x418) &= 0xFDu;
            *(uint8_t *)((char *)game_object + 0x418) |= 2 * invulnerable;
        };

        const auto character = jc::CPlayerManager::GetLocalPlayerCharacter();
        if (character) {
            const auto vehicle = character->GetVehiclePtr();

            if (vehicle) {
                const bool char_invulnerable = (*(uint8_t *)((char *)character + 0x418) & 2);
                const bool veh_invulnerable  = (*(uint8_t *)((char *)vehicle + 0x418) & 2);

                // player is invulnerable, but the vehicle is not - set the vehicle invulnerable.
                if (char_invulnerable && !veh_invulnerable) {
                    SetInvulnerable(vehicle, true);
                    invul_vehicle = vehicle;
                }
                // player is not invulnerable, but the vehicle is, AND the vehicle matches the vehicle we set
                // invulnerability on (this should prevent weird cases where a vehicle was made invulnerable by the
                // game)
                else if ((!char_invulnerable && veh_invulnerable) && vehicle == invul_vehicle) {
                    goto make_vulnerable;
                }
            }
            // player not in a vehicle and we manually set invulnerability on a vehicle - set the vehicle vulnerable
            else if (invul_vehicle) {
            make_vulnerable:
                SetInvulnerable(invul_vehicle, false);
                invul_vehicle = nullptr;
            }
        }
    }
}

bool InitPatchesAndHooks()
{
    // basic sanity check, ensure the game version is what we are expecting.
    // this will prevent crashes if the game updates, but someone is using an old version of this mod.
    if (strcmp((const char *)GetAddress(SANITY_CHECK), "Avalanche Engine") != 0) {
#ifdef DEBUG
        __debugbreak();
#endif
        return false;
    }

    // enable quick start
    meow_hook::put(GetAddress(VAR_QUICK_START), true);
    meow_hook::put(GetAddress(IS_INTRO_SEQUENCE_COMPLETE), 0x90C301B0);
    meow_hook::put(GetAddress(IS_INTRO_MOVIE_COMPLETE), 0x90C301B0);

    // WndProc
    pfn_WndProc = MH_STATIC_DETOUR(GetAddress(WND_PROC), WndProc);

    // Graphics::Flip
    Graphics::pfn_Flip = MH_STATIC_DETOUR(GetAddress(GRAPHICS_FLIP), Graphics::GraphicsFlipCallback);

    // CPlayerManager::Update
    pfn_Update = MH_STATIC_DETOUR(GetAddress(PLAYER_MANAGER_UPDATE), PlayerManagerUpdate);

    // override CFiringModule::ConsumeAmmo to fix unlimited ammo not being applied to vehicles
    pfn_ConsumeAmmo = MH_STATIC_DETOUR(GetAddress(FIRING_MODULE_CONSUME_AMMO), FiringModuleConsumeAmmo);
    return true;
}
}; // namespace jc
