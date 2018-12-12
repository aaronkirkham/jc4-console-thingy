#pragma once

#include "command.h"

class SpawnCommand : public ICommand
{
  public:
    std::array<const char *, 93> m_Hints = {
        "v000_car_atv_civilian_01",
        "v001_car_forklift_industrial",
        "v002_car_vintagesuv_civilian",
        "v002_car_vintagesuv_rebel",
        "v003_car_crane_industrial",
        "v004_car_articulatedtruck_commercial_cargo_trailer",
        "v004_car_articulatedtruck_commercial_randomtrailer",
        "v005_car_wheelloader_industrial",
        "v008_car_taxitrike_civilian",
        "v010_car_oldtractor_civilian_01",
        "v011_car_oldtwodoorhatch_civilian_01",
        "v012_car_apc_military_01",
        "v012_car_apc_rebel_01",
        "v013_car_armoredtransport_military_01",
        "v013_car_armoredtransport_rebel_01",
        "v014_car_offroadtruck_military_01",
        "v014_car_offroadtruck_military_01,vehicle",
        "v014_car_offroadtruck_rebel_01",
        "v014_car_offroadtruck_rebel_01,vehicle",
        "v015_car_cannontruck_military",
        "v016_car_armoredarticulatedtruck_military",
        "v017_car_vintagemuscle_civilian,vehicle",
        "v018_car_monstertruck_civilian_01",
        "v019_car_modernlimo_civilian_01",
        "v021_car_sportsmuscle_civilian_01,vehicle",
        "v024_car_ecosuper_civilian_01",
        "v029_car_smallmodernsedan_civilian_01",
        "v029_car_smallmodernsedan_civilian_01,vehicle",
        "v034_car_oldtruck_commercial_01",
        "v035_car_modernvan_civic_ambulance_01",
        "v035_car_modernvan_garland",
        "v036_car_modernpickup_civilian_01",
        "v037_car_modernbus_commercial_01",
        "v039_car_oldcampervan_civilian_01",
        "v040_car_oldcompact_civilian_01,vehicle",
        "v046_car_racingbuggy_military",
        "v050_car_toyjeep_civilian",
        "v101_boat_smalljet_military",
        "v101_boat_smalljet_rebel",
        "v102_boat_heavypatrol_military_01",
        "v102_boat_heavypatrol_rebel_01",
        "v103_boat_corvette_military_01",
        "v105_boat_sailboat_civilian_01",
        "v106_boat_motoryacht_civilian_01",
        "v107_boat_racingboat_civilian_01",
        "v109_boat_ferry_commercial",
        "v110_boat_jetski_civilian_01",
        "v201_helicopter_mediumattack_military_01",
        "v201_helicopter_mediumattack_rebel_01",
        "v202_helicopter_heavytroop_military_01",
        "v202_helicopter_heavytroop_rebel_01",
        "v203_helicopter_lightattack_military_01",
        "v203_helicopter_lightattack_rebel_01",
        "v204_helicopter_heavyassault_military_01",
        "v204_helicopter_heavyassault_rebel_01",
        "v205_helicopter_utility_civilian_01",
        "v205_helicopter_utility_commercial_news",
        "v250_helicopter_mediumattackdrone_military",
        "v251_helicopter_rocketdrone_military",
        "v301_bike_combatdirt_military_01",
        "v301_bike_combatdirt_rebel_01",
        "v303_bike_modernsuper_civilian_01",
        "v304_bike_modernsport_civilian_01",
        "v305_bike_oldroad_civilian_01",
        "v306_bike_modernroad_civilian_01",
        "v307_bike_oldmoped_civilian_01",
        "v308_bike_trials_civilian_01",
        "v400_plane_fighterjet_military_01",
        "v400_plane_fighterjet_rebel_01",
        "v401_plane_cargotransport_military_01",
        "v401_plane_cargotransport_rebel_01",
        "v402_plane_fighterbomber_military_01",
        "v402_plane_fighterbomber_rebel_01",
        "v403_plane_microjet_military",
        "v403_plane_microjet_rebel",
        "v404_plane_privatejet_civilian",
        "v405_plane_commercialcargo_commercial",
        "v406_plane_smallprop_civilian_01",
        "v407_plane_mediumprop_civilian_01",
        "v408_plane_ultralight_civilian",
        "v500_train_industrialengine_industrial_01",
        "v700_balloon_dirigible_civilian",
        "v700_balloon_dirigible_collectible",
        "v800_treaded_modernheavytank_military_01",
        "v800_treaded_modernheavytank_rebel_01",
        "v801_treaded_mediumtank_military_01",
        "v801_treaded_mediumtank_rebel_01",
        "v802_treaded_aatank_military",
        "v803_treaded_flexturrettank_military",
        "v903_trailer_cargo_commercial",
        "v904_trailer_smallmobileweapon_military",
        "v904_trailer_smallmobileweapon_rebel",
        "v907_trailer_radarjammer_military",
    };

    virtual const char *GetCommand() override
    {
        return "spawn";
    }

    virtual bool Handler(const std::string &arguments) override
    {
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

    virtual std::vector<std::string> GetHints(const std::string &arguments) override
    {
        std::vector<std::string> result;

        std::copy_if(m_Hints.begin(), m_Hints.end(), std::back_inserter(result),
                     [&](const std::string &item) { return item.find(arguments) != std::string::npos; });

        return result;
    }
};
