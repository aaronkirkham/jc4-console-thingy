#pragma once

#include "vector.h"

#include "addresses.h"

#pragma pack(push, 1)
namespace jc
{
class CGameWorld
{
  public:
    static CGameWorld &instance()
    {
        return **(CGameWorld **)GetAddress(INST_GAME_WORLD);
    }

  public:
    virtual ~CGameWorld()                                               = 0;
    virtual void function_1()                                           = 0;
    virtual void function_2()                                           = 0;
    virtual void function_3()                                           = 0;
    virtual void function_4()                                           = 0;
    virtual void function_5()                                           = 0;
    virtual void function_6()                                           = 0;
    virtual void function_7()                                           = 0;
    virtual void function_8()                                           = 0;
    virtual void function_9()                                           = 0;
    virtual void function_10()                                          = 0;
    virtual void function_11()                                          = 0;
    virtual void function_12()                                          = 0;
    virtual void TeleportPlayer(CMatrix4f *world, uint32_t *state, bool use_load_state, bool use_fade,
                                float fade_in_time, float fade_out_time, uint32_t flags, void *callback,
                                bool use_loading_screen, void *unknown) = 0;
    // @NOTE: TeleportPlayerInstant is not useful for us. Only usable inside post-sim context and the terrain/collisions
    // are not ready instantly.
    virtual void TeleportPlayerInstant(CMatrix4f *world, uint32_t *state, uint32_t flags) = 0;
    virtual bool IsTeleporting()                                                          = 0;
};
}; // namespace jc
#pragma pack(pop)
