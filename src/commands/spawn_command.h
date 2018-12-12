#pragma once

#include "command.h"

class SpawnCommand : ICommand
{
  public:
    static bool Handler(const std::string &arguments)
    {
        // v402_plane_fighterbomber_rebel_01
        // v402_plane_fighterbomber_military_01
        // ply.unlimitedammo.enable
        // ply.unlimitedammo.disable
        // ply.ammo.givemax
        // spawn weapon,shotgun

        auto  local_player = jc::CPlayerManager::instance().m_localPlayer;
        auto  transform    = local_player->m_character->m_transform;
        auto &aimpos       = local_player->m_aimControl->m_aimPos;

        transform.m[3].x = aimpos.x;
        transform.m[3].y = aimpos.y;
        transform.m[3].z = aimpos.z;

        jc::CSpawnSystem::instance().Spawn(arguments, transform, [](const jc::spawned_objects &objects, void *) {
            //
        });

        return true;
    }
};
