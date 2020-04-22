#include "addresses.h"

namespace jc
{
static uintptr_t g_Address[Address::COUNT] = {};

void InitAddresses(bool is_steam)
{
    g_Address[SANITY_CHECK] = is_steam ? 0x141e7ee40 : 0x141e84958;
    g_Address[INST_CLOCK] = is_steam ? 0x142c846b0 : 0x142c8aaa8;
    g_Address[INST_CHARACTER_MANAGER] = is_steam ? 0x142cb1d40 : 0x142cb8110;
    g_Address[INST_GAME_WORLD] = is_steam ? 0x142cb0a48 : 0x142cb6e08;
    g_Address[INST_INPUT_MANAGER] = is_steam ? 0x142ce1af0 : 0x142ce7f30;
    g_Address[INST_PLAYER_MANAGER] = is_steam ? 0x142cb2388 : 0x142cb8758;
    g_Address[INST_RENDER_ENGINE] = is_steam ? 0x142c84be8 : 0x142c8afd8;
    g_Address[INST_SPAWN_SYSTEM] = is_steam ? 0x142cb1d20 : 0x142cb80f0;
    g_Address[INST_UI_MANAGER] = is_steam ? 0x142cb7dc8 : 0x142cbe190;
    g_Address[INST_HNPKWORLD] = is_steam ? 0x142ce3f30 : 0x142cea370;
    g_Address[INST_WORLDTIME] = is_steam ? 0x142cafdb0 : 0x142cb6170;
    g_Address[VAR_QUICK_START] = is_steam ? 0x142cb8f40 : 0x142cbf304;
    g_Address[IS_INTRO_SEQUENCE_COMPLETE] = is_steam ? 0x140e935b0 : 0x140e9a950;
    g_Address[IS_INTRO_MOVIE_COMPLETE] = is_steam ? 0x140e93530 : 0x140e9a8d0;
    g_Address[WND_PROC] = is_steam ? 0x140c7fb50 : 0x140c86830;
    g_Address[GRAPHICS_FLIP] = is_steam ? 0x140fa2c70 : 0x140fa9fd0;
    g_Address[PLAYER_MANAGER_UPDATE] = is_steam ? 0x1498aa770 : 0x14eb62e10;
    g_Address[FIRING_MODULE_CONSUME_AMMO] = is_steam ? 0x140728840 : 0x14072c510;
    g_Address[VAR_GAME_STATE] = is_steam ? 0x142cb8f24 : 0x142cbf2d4;
    g_Address[VAR_SUSPEND_GAME] = is_steam ? 0x142cbdaf0 : 0x142cc3f90;
    g_Address[ALLOC] = is_steam ? 0x140a51dc0 : 0x140a56d90;
    g_Address[FREE] = is_steam ? 0x141af8e1c : 0x141b00b5c;
    g_Address[GET_DEFAULT_PLATFORM_ALLOCATOR] = is_steam ? 0x140f236f0 : 0x140f2ac00;
    g_Address[PLATFORM_ALLOCATOR_FREE] = is_steam ? 0x140838070 : 0x14083c9b0;
    g_Address[CHARACTER_MODEL_COPY_PARTS] = is_steam ? 0x14026e9b0 : 0x14026f7f0;
    g_Address[CHARACTER_MODEL_SET_PROPERTIES] = is_steam ? 0x14028c790 : 0x14028d4e0;
    g_Address[CHARACTER_MODEL_REBUILD_MODEL] = is_steam ? 0x140287840 : 0x140288590;
    g_Address[CHARACTER_GET_VEHICLE_PTR] = is_steam ? 0x14054fd50 : 0x140551a10;
    g_Address[SEND_EVENT] = is_steam ? 0x140286930 : 0x140287680;
    g_Address[SPAWN_SYSTEM_SPAWN] = is_steam ? 0x140badc60 : 0x140bb4290;
    g_Address[SPAWN_SYSTEM_PARSE_TAGS] = is_steam ? 0x1499d0740 : 0x14ec92981;
    g_Address[SPAWN_SYSTEM_GET_MATCHING_RESOURCES] = is_steam ? 0x140b88770 : 0x140b8e7a0;
    g_Address[WORLDTIME_SET_TIME] = is_steam ? 0x140322720 : 0x140324070;
    g_Address[INPUT_LOST_FOCUS] = is_steam ? 0x140fc8560 : 0x140fcde60;
    g_Address[INPUT_GAIN_FOCUS] = is_steam ? 0x140fc84b0 : 0x140fcddb0;
    g_Address[ENTITY_PROVIDER_CTOR] = is_steam ? 0x14025f860 : 0x140260710;
    g_Address[ENTITY_PROVIDER_LOAD_RESOURCES] = is_steam ? 0x140284870 : 0x140285590;
    g_Address[ENTITY_PROVIDER_UPDATE_INTERNAL] = is_steam ? 0x140294f20 : 0x140295cd0;
    g_Address[DRAW_SKIN_BATCHES] = is_steam ? 0x140d1a150 : 0x140d21090;
    g_Address[RUNTIME_CONTAINER_GET_HASH] = is_steam ? 0x140091360 : 0x140090f60;
    g_Address[VTABLE_RENDERBLOCKCHARACTER] = is_steam ? 0x141eac3a0 : 0x141eb1ec0;
    g_Address[VTABLE_RENDERBLOCKCHARACTERSKIN] = is_steam ? 0x141eac550 : 0x141eb2070;
    g_Address[VTABLE_RENDERBLOCKGENERAL] = is_steam ? 0x141ebdf50 : 0x141ec3a80;
}

uintptr_t GetAddress(Address address)
{
    return g_Address[address];
}
}; // namespace jc