#pragma once

#include "command.h"

class SpawnCommand : public ICommand
{
  public:
    virtual const char *GetCommand() override
    {
        return "spawn";
    }

    virtual bool Handler(const std::string &arguments) override
    {
        if (arguments == "rico_debug" || arguments == "rico_preview_debug" || arguments == "rico_cow_skin"
            || arguments == "rico_cow_skin_debug") {
            return false;
        }


        auto  local_player = jc::CPlayerManager::instance().m_localPlayer;
        auto  transform    = local_player->m_character->m_worldTransform;
        auto &aimpos       = local_player->m_aimControl->m_aimPos;

        transform.m[3].x = aimpos.x;
        transform.m[3].y = aimpos.y + 1.0f;
        transform.m[3].z = aimpos.z;

        // figure out if the player wants to spawn more than one thing
        if (arguments.find("*", 0) != std::string::npos) {
            // identify the multiplier and turn it into something we can use, then create a string without it
            // also identify the spacer to enable spawning of bigger things
            // limiting the size of the substring limits max spawned entities, although the game seems to limit this itself
            size_t multiplier_position = arguments.find("*", 0);
            size_t spacer_position     = arguments.find("/", 0);
            std::string truncated_args = arguments.substr(0, multiplier_position - 1);

            int multiplier             = std::stoi(arguments.substr(multiplier_position + 1, 4)); 
            int spacer                 = 5;

            if (spacer_position != std::string::npos) {
                spacer = std::stoi(arguments.substr(spacer_position + 1, std::string::npos));
            }

            int square_root            = (int) (std::sqrt((double) multiplier) + 0.5f);
            // do the actual spawning
            for (int i = 0; i < multiplier; i++) {
                // spawn things in as square a rectangle as possible
                if ((i% square_root == 0) && (i != 0)) {
                    transform.m[3].x = transform.m[3].x + spacer;
                    transform.m[3].z = transform.m[3].z - (square_root * spacer);
                } else {
                    transform.m[3].z = transform.m[3].z + spacer;
                }
                jc::CSpawnSystem::instance().Spawn(truncated_args, transform,
                                                   [](const jc::spawned_objects &objects, void *) {});
            }
            
        } else {

            // NOTE(aaronlad): spawn flag 0x8000 will prevent auto despawn
            jc::CSpawnSystem::instance().Spawn(arguments, transform, [](const jc::spawned_objects &objects, void *) {});
        }

        return true;
    }

    virtual std::vector<std::string> GetHints(const std::string &arguments) override
    {
        static std::array hints{
            // Base game - Land vehicles
            "v000_car_atv_civilian_01",
            "v000_car_atv_javi",
            "v001_car_forklift_industrial",
            "v002_car_vintagesuv_civilian",
            "v002_car_vintagesuv_javi",
            "v002_car_vintagesuv_rebel",
            "v003_car_crane_industrial",
            "v004_car_articulatedtruck_commercial_01",
            "v004_car_articulatedtruck_commercial_cargo_trailer",
            "v004_car_articulatedtruck_commercial_lowloader_trailer",
            "v004_car_articulatedtruck_commercial_radarjammer",
            "v004_car_articulatedtruck_commercial_smallmobileweapon_trailer",
            "v005_car_wheelloader_industrial",
            "v006_car_conveyorcrane_industrial",
            "v008_car_taxitrike_civilian",
            "v009_car_armoredtruck_commercial_01",
            "v010_car_oldtractor_civilian_01",
            "v011_car_oldtwodoorhatch_civilian_01",
            "v011_car_oldtwodoorhatch_sargento",
            "v012_car_apc_military_01",
            "v012_car_apc_rebel_01",
            "v013_car_armoredtransport_military_01",
            "v013_car_armoredtransport_rebel_01",
            "v014_car_offroadtruck_military_01",
            "v014_car_offroadtruck_rebel_01",
            "v015_car_cannontruck_military",
            "v015_car_cannontruck_rebel",
            "v016_car_armoredarticulatedtruck_military",
            "v016_car_armoredarticulatedtruck_rebel",
            "v017_car_vintagemuscle_bomb_special",
            "v017_car_vintagemuscle_civilian",
            "v017_car_vintagemuscle_civilian_02",
            "v018_car_monstertruck_bomb_special",
            "v018_car_monstertruck_civilian_01",
            "v019_car_modernlimo_bomb_special",
            "v019_car_modernlimo_civilian_01",
            "v020_car_moderngrandtourer_civilian_01",
            "v021_car_sportsmuscle_bomb_special",
            "v021_car_sportsmuscle_civilian_01",
            "v022_car_moderncircuitracer_bomb_special",
            "v022_car_moderncircuitracer_civilian_01",
            "v023_car_racingsuper_bomb_special",
            "v023_car_racingsuper_civilian_01",
            "v023_car_racingsuper_racing_01",
            "v024_car_ecosuper_bomb_special",
            "v024_car_ecosuper_civilian_01",
            "v024_car_ecosuper_racing_01",
            "v025_car_vintagesuper_bomb_special",
            "v025_car_vintagesuper_civilian_01",
            "v026_car_vintagesports_bomb_special",
            "v026_car_vintagesports_civilian_01",
            "v027_car_hurricanetruck_civilian",
            "v027_car_hurricanetruck_tesla",
            "v027_car_hurricanetruck_transport",
            "v029_car_smallmodernsedan_civilian_01",
            "v029_car_smallmodernsedan_civilian_02",
            "v030_car_oldmini_civilian_01",
            "v030_car_oldmini_civilian_02",
            "v031_car_racinghothatch_civilian_01",
            "v031_car_racinghothatch_encounter_takedown",
            "v032_car_luxurysportssedan_civilian_01",
            "v033_car_modernmini_civilian_01",
            "v033_car_modernmini_civilian_02",
            "v034_car_oldtruck_commercial_01",
            "v034_car_oldtruck_commercial_cargo_barrel",
            "v034_car_oldtruck_commercial_cargo_barrel_mission_special",
            "v034_car_oldtruck_commerical_cargo_random",
            "v034_car_oldtruck_commerical_cargo_sargento_intro",
            "v035_car_modernvan_civic_ambulance_01",
            "v035_car_modernvan_commerical_01",
            "v035_car_modernvan_garland",
            "v036_car_modernpickup_civilian_01",
            "v036_car_modernpickup_civilian_02",
            "v037_car_modernbus_commerical_01",
            "v038_car_modernsuv_civilian_01",
            "v038_car_modernsuv_civilian_02",
            "v039_car_oldcampervan_civilian_01",
            "v039_car_oldcampervan_civilian_02",
            "v040_car_oldcompact_civilian_01",
            "v040_car_oldcompact_sargento",
            "v042_car_racingsedan_civilian",
            "v042_car_racingsedan_racing_01",
            "v045_car_minetruck_commercial_01",
            "v046_car_racingbuggy_civilian",
            "v046_car_racingbuggy_military",
            "v046_car_racingbuggy_racing_01",
            "v050_car_toyjeep_civilian",

            // Base game - Bikes
            "v301_bike_combatdirt_military_01",
            "v301_bike_combatdirt_rebel_01",
            "v303_bike_modernsuper_bomb_special",
            "v303_bike_modernsuper_civilian_01",
            "v304_bike_modernsport_bomb_special",
            "v304_bike_modernsport_civilian_01",
            "v305_bike_oldroad_civilian_01",
            "v306_bike_modernroad_civilian_01",
            "v307_bike_oldmoped_civilian_01",
            "v308_bike_trials_civilian_01",
            "v308_bike_trials_javi",

            // Base game - Treaded
            "v800_treaded_modernheavytank_military_01",
            "v800_treaded_modernheavytank_rebel_01",
            "v801_treaded_mediumtank_military_01",
            "v801_treaded_mediumtank_rebel_01",
            "v802_treaded_aatank_military",
            "v802_treaded_aatank_rebel",
            "v803_treaded_flexturrettank_military",
            "v803_treaded_flexturrettank_rebel",
            "v804_treaded_snowmobile_civilian",
            "v804_treaded_snowmobile_civilian_mission_special",

            // Base game - Sea vehicles
            "v100_boat_fanboat_civilian",
            "v101_boat_smalljet_military",
            "v101_boat_smalljet_rebel",
            "v102_boat_heavypatrol_lightning",
            "v102_boat_heavypatrol_military_01",
            "v102_boat_heavypatrol_rebel_01",
            "v103_boat_corvette_military_01",
            "v103_boat_corvette_rebel_01",
            "v104_boat_landingtransport_commercial",
            "v104_boat_landingtransport_commercial_lightningrod_escort",
            "v104_boat_landingtransport_commercial_signal_jammer",
            "v104_boat_landingtransport_rebel",
            "v105_boat_sailboat_civilian_01",
            "v106_boat_motoryacht_bomb_special",
            "v106_boat_motoryacht_civilian_01",
            "v107_boat_racingboat_bomb_special",
            "v107_boat_racingboat_civilian_01",
            "v108_boat_largeoldfishing_civilian_01",
            "v108_boat_largeoldfishing_civilian_01_garland_intro",
            "v109_boat_ferry_commercial",
            "v109_boat_ferry_commercial_garlandintro",
            "v110_boat_jetski_civilian_01",

            // Base game - Helicopters
            "v200_helicopter_heavylift_military",
            "v200_helicopter_heavylift_military_chaos_sphere_tank",
            "v200_helicopter_heavylift_military_quest_special",
            "v200_helicopter_heavylift_rebel_01",
            "v200_helicopter_heavylift_rebel_mission_special",
            "v201_helicopter_mediumattack_military_01",
            "v201_helicopter_mediumattack_rebel_01",
            "v202_helicopter_heavytroop_gabriela",
            "v202_helicopter_heavytroop_military_01",
            "v202_helicopter_heavytroop_mira",
            "v202_helicopter_heavytroop_rebel_01",
            "v203_helicopter_lightattack_military_01",
            "v203_helicopter_lightattack_rebel_01",
            "v204_helicopter_heavyassault_military_01",
            "v204_helicopter_heavyassault_rebel_01",
            "v205_helicopter_utility_civilian_01",
            "v205_helicopter_utility_commercial_news",
            "v205_helicopter_utility_garland",
            "v206_helicopter_bubblescout_civilian_01",
            "v206_helicopter_bubblescout_javi",
            "v250_helicopter_mediumattackdrone_lead_military",
            "v250_helicopter_mediumattackdrone_military",
            "v250_helicopter_mediumattackdrone_rebel",
            "v250_helicopter_mediumattackdrone_tornado",
            "v251_helicopter_rocketdrone_military",
            "v251_helicopter_rocketdrone_rebel",
            "v252_helicopter_suicidedrone_military",
            "v252_helicopter_suicidedrone_rebel",
            "v253_helicopter_decoydrone_preorder",
            "v253_helicopter_decoydrone_rebel",
            "v254_helicopter_guarddrone_military",
            "v254_helicopter_guarddrone_rebel",
            "v270_helicopter_agencydrone_debug",
            "v270_helicopter_agencydrone_rebel",

            // Base game - Planes
            "v400_plane_fighterjet_military_01",
            "v400_plane_fighterjet_rebel_01",
            "v401_plane_cargotransport_military_01",
            "v401_plane_cargotransport_rebel_01",
            "v401_plane_cargotransport_signal_jammer",
            "v402_plane_fighterbomber_military_01",
            "v402_plane_fighterbomber_rebel_01",
            "v403_plane_microjet_military",
            "v403_plane_microjet_rebel",
            "v404_plane_privatejet_civilian",
            "v405_plane_commercialcargo_commercial",
            "v406_plane_smallprop_civilian_01",
            "v407_plane_mediumprop_civilian_01",
            "v408_plane_ultralight_civilian",

            // Base game - Balloon
            "v700_balloon_dirigible_civilian",
            "v700_balloon_dirigible_collectible",
            "v700_balloon_dirigible_military_01",
            "v700_balloon_dirigible_node_science_01",

            // Base game - Trains
            "v500_train_industrialengine_industrial",
            "v500_train_industrialengine_rebel",
            "v501_train_containercarriage_industrial",
            "v501_train_containercarriage_rebel",
            "v502_train_armoredengine_military_01",
            "v502_train_armoredengine_military_01_mission_special_steal_the_weapon_tech",
            "v502_train_armoredengine_military_01_node_military_05",
            "v502_train_armoredengine_military_02",
            "v502_train_armoredengine_military_03",
            "v502_train_armoredengine_military_04",
            "v502_train_armoredengine_rebel",
            "v502_train_armoredengine_rebel_02",
            "v502_train_armoredengine_rebel_04",
            "v502_train_armoredengine_rebel_mission_special_outro",
            "v502_train_armoredengine_rebel_mission_special_sandstorm_finale",
            "v503_train_armoredcargocarriage_military_catwalk",
            "v503_train_armoredcargocarriage_military_containers",
            "v503_train_armoredcargocarriage_military_empty",
            "v503_train_armoredcargocarriage_rebel",
            "v503_train_armoredcargocarriage_rebel_containers",
            "v503_train_armoredcargocarriage_rebel_empty",
            "v503_train_armoredcargocarriage_rebel_empty_decouple_immune",
            "v504_train_armoredfuelcarriage_military",
            "v504_train_armoredfuelcarriage_rebel",
            "v504_train_armoredfuelcarriage_rebel_not_targetable",
            "v505_train_armoredweaponcarriage_military_01",
            "v505_train_armoredweaponcarriage_rebel",
            "v506_train_armoredhowitzer_base",
            "v506_train_armoredhowitzer_military",
            "v506_train_armoredhowitzer_military_covered",
            "v506_train_armoredhowitzer_rebel",
            "v506_train_armoredhowitzer_decouple_immune",

            // Base game - Trailers
            "v901_trailer_cartransport_commercial",
            "v902_trailer_lowloader_commerical",
            "v902_trailer_lowloader_mobile_radar_folded",
            "v902_trailer_lowloader_mobile_radar_folded_sargento_aiproxy_enabled",
            "v902_trailer_lowloader_mobile_radar_upright",
            "v902_trailer_lowloader_mobile_radar_upright_sargent_aiproxy_enabled",
            "v903_trailer_cargo_commerical",
            "v904_trailer_smallmobileweapon_military",
            "v904_trailer_smallmobileweapon_rebel",
            "v907_trailer_radarjammer_military",

            // Base game - Weapons
            "wpn_000_assault_rifle",
            "wpn_001_assault_rifle",
            "wpn_003_assault_rifle",
            "wpn_010_machinegun",
            "wpn_011_machinegun",
            "wpn_020_shotgun",
            "wpn_021_shotgun",
            "wpn_022_shotgun",
            "wpn_030_sniper_rifle",
            "wpn_031_sniper_rifle",
            "wpn_032_sniper_rifle",
            "wpn_040_combat_rifle",
            "wpn_050_smg",
            "wpn_060_rpg",
            "wpn_061_rpg",
            "wpn_063_rpg",
            "wpn_070_glauncher",
            "wpn_071_mlauncher",
            "wpn_080_experimental",
            "wpn_081_experimental",
            "wpn_100_railgun",
            "w206_mounted_capstone_m2hm",
            "wpn_201_minigun",
            "wpn_201_minigun_aoc",
            "wpn_201_minigun_mount",
            "wpn_201_minigun_mount_rebel",
            "wpn_201_minigun_mount_military",
            "w301_grenade",
            "w301_frag_grenade",
            "w303_concussion_grenade",
            "w304_death_dropped_grenade",
            "wpn_202_cannon",
            "wpn_202_cannon_rebel",
            "wpn_202_cannon_aoc",
            "wpn_203_aa_gun",
            "wpn_203_aa_gun_rebel",
            "wpn_203_aa_gun_aoc",
            "wpn_204_mortar_mounted",
            "wpn_204_mortar_mounted_rebel",
            "wpn_204_mortar_mounted_aoc",
            "illapa_defence_weapon",
            "cow_gun",
            "the_pan",

            // DLC DigitalDeluxe
            "wpn_990_premium_wingsuit_bullet_streamer",
            "wpn_991_premium_wingsuit_dragon_breath",
            "v403_plane_microjet_preorder_01",

            // DLC DeathStalker
            "v017_car_vintagemuscle_rico",
            "v046_car_racingbuggy_preorder_01",
            "wpn_901_signature_rifle",

            // DLC GoldenGear
            "wpn_902_golden_shotgun",

            // DLC Renegade
            "wpn_903_renegade_assault_rifle",

            // DLC Neon racer
            "v024_car_ecosuper_preorder_01",

            // DLC Toy Vehicle pack
            "v122_boat_toypatrol_garage",
            "v420_plane_toybomber_garage",
            "v820_treaded_toytank_garage",

            // DLC Soaring Speed pack
            "v053_car_hotrod_garage",
            "v053_car_hotrod_ratrod",
            "v086_car_flyingcar_garage",

            // DLC Legacy pack
            "v084_car_icecreamtruck_garage",
            "v085_car_explosivetrike_garage",
            "v117_boat_duck_garage",
            "wpn_905_clusterbomb",

            // DLC Adversary pack
            "v422_plane_vtoljet_garage",
            "v081_car_interceptor",

            // DLC Sea Dogs pack
            "v080_car_galleonfloat_garage",
            "v850_car_hovercraft_armored_garage",

            // DLC Shark & Bark pack
            "v082_car_shark_garage",
            "v083_car_dog_garage",
            
            // DLC Brawler Mech
            "v600_mech_loader_garage",

            // DLC Daredevils
            "v070_car_offroadtruckdlc1_level_0",
            "v070_car_offroadtruckdlc1_level_1",
            "v070_car_offroadtruckdlc1_level_2",
            "v070_car_offroadtruckdlc1_level_3",
            
            "v071_car_modernpickupdlc1_level_0",
            "v071_car_modernpickupdlc1_level_1",
            "v071_car_modernpickupdlc1_level_2",
            "v071_car_modernpickupdlc1_level_3",
            
            "v072_car_vintagesuvdlc1_debug",
            "v072_car_vintagesuvdlc1_level_0",
            "v072_car_vintagesuvdlc1_level_1",
            "v072_car_vintagesuvdlc1_level_2",
            "v072_car_vintagesuvdlc1_level_3",

            "v073_car_luxurysportssedandlc1_debug",
            "v073_car_luxurysportssedandlc1_level_0",
            "v073_car_luxurysportssedandlc1_level_1",
            "v073_car_luxurysportssedandlc1_level_2",
            "v073_car_luxurysportssedandlc1_level_3",

            "v074_car_ecosuperdlc1_debug",
            "v074_car_ecosuperdlc1_level_0",
            "v074_car_ecosuperdlc1_level_1",
            "v074_car_ecosuperdlc1_level_2",
            "v074_car_ecosuperdlc1_level_3",

            "v075_car_vintagesuperdlc1_debug",
            "v075_car_vintagesuperdlc1_level_0",
            "v075_car_vintagesuperdlc1_level_1",
            "v075_car_vintagesuperdlc1_level_2",
            "v075_car_vintagesuperdlc1_level_3",

            "v076_car_racingbuggydlc1_debug",
            "v076_car_racingbuggydlc1_level_0",
            "v076_car_racingbuggydlc1_level_1",
            "v076_car_racingbuggydlc1_level_2",
            "v076_car_racingbuggydlc1_level_3",

            "v077_car_offroadracingsedandlc1_debug",
            "v077_car_offroadracingsedandlc1_level_0",
            "v077_car_offroadracingsedandlc1_level_1",
            "v077_car_offroadracingsedandlc1_level_2",
            "v077_car_offroadracingsedandlc1_level_3",

            "v078_car_monstertruckdlc1_debug",
            "v078_car_monstertruckdlc1_level_0",
            "v078_car_monstertruckdlc1_level_1",
            "v078_car_monstertruckdlc1_level_2",
            "v078_car_monstertruckdlc1_level_3",

            "v051_car_battlemuscle_allgold_level_3",
            "v051_car_battlemuscle_collectible_level_3",
            "v051_car_battlemuscle_daredevil",
            "v051_car_battlemuscle_level_0",
            "v051_car_battlemuscle_metal01",
            "v051_car_battlemuscle_rust01",
            "v051_car_battlemuscle_stripe01",
            "v051_car_battlemuscle_tiger01",

            "v051_car_battlemuscle_deathraceace_level_1",
            "v051_car_battlemuscle_deathraceace_level_2",
            "v051_car_battlemuscle_deathraceace_level_3",

            "v051_car_battlemuscle_intro_level_1",
            "v051_car_battlemuscle_intro_level_2",
            "v051_car_battlemuscle_intro_level_3",

            "v051_car_battlemuscle_outro_level_1",
            "v051_car_battlemuscle_outro_level_2",
            "v051_car_battlemuscle_outro_level_3",

            "v051_car_battlemuscle_rampagerallyace_level_1",
            "v051_car_battlemuscle_rampagerallyace_level_2",
            "v051_car_battlemuscle_rampagerallyace_level_3",

            "v051_car_battlemuscle_survivalraceace_level_1",
            "v051_car_battlemuscle_survivalraceace_level_2",
            "v051_car_battlemuscle_survivalraceace_level_3",
            
            "daredevil",

            // DLC Demons
            "wpn_102_crossbow",
            "wpn_808_combat_rifle_seed_gun",

            "dlc2_substrate_weapon_aa_gun_01",
            "dlc2_substrate_weapon_aa_tank_01",
            "dlc2_substrate_weapon_apc_01",
            "dlc2_substrate_weapon_cannon_truck_01",
            "dlc2_substrate_weapon_heli_light_01",
            "dlc2_substrate_weapon_modern_tank_01",
            "dlc2_substrate_weapon_mortar_01",

            "dlc2_substrate_weapon_aa_gun_01_outro",
            "dlc2_substrate_weapon_aa_tank_01_outro",
            "dlc2_substrate_weapon_apc_01_outro",
            "dlc2_substrate_weapon_cannon_truck_01_outro",
            "dlc2_substrate_weapon_modern_tank_01_outro",
            "dlc2_substrate_weapon_mortar_01_outro",

            "demon",

            // DLC Danger
            "v002_car_vintagesuv_rebel_mission_special_dlc3_intro",
            "v851_car_hovercraft_agency",
            "v851_car_hovercraft_agency_mission_special_dlc3_outro",
            "v123_boat_agencyspy_agency",
            "v221_helicopter_agencyspy_agency",
            "v270_helicopter_agencydrone_agency",
            "v270_helicopter_agencydrone_rebel",
            "v908_trailer_agencysmallmobileweapon",

            "wpn_100_railgun_miller",
            "wpn_205_agency_aa_gun",
            "wpn_450_agency_handcannon",
            "wpn_460_agency_smg",
            "wpn_461_agency_sniper",
            "wpn_463_magnesis",

            "agent_miller",
            "sheldon_dlc3",
            "agency_footsoldier_enemy_female_001",
            "agency_footsoldier_enemy_male_001",
            "agency_sniper_enemy_female_001",
            "agency_sniper_enemy_male_001",
            "agency_agent_enemy_female_001",
            "agency_agent_enemy_male_001",
            "agency_agent_enemy_vip_female01",
            "agency_agent_enemy_vip_female02",
            "agency_agent_enemy_vip_female03",
            "agency_agent_enemy_vip_male01",
            "agency_agent_enemy_vip_male02",
            "agency_agent_enemy_vip_male03",
            "agency_agent_enemy_vip_titan01",

            "agency_cargo_missile",
            "dlc3_water_mine",

            // Base game - Characters
            "private_enemy_001",
            "private_enemy_002",
            "elite_enemy_001",
            "elite_paratrooper",
            "super_elite_enemy_001",
            "titan_enemy_001",
            "ghost_enemy_001",
            "grenadier_enemy_001",
            "machinegunner_enemy_001",
            "rpg_enemy_001",
            "shielder_enemy_001",
            "sniper_enemy_001",

            // Base game - Animals
            "cow",
            "bull",
            "deer_doe",
            "deer_buck",
            "dog",
            "goat",
            "goat_bhuj",
            "tapir",
            "llama",
            "llama_02",
            "llama_03_sheared",
            "capybara",
            "domestic_pig",
            "feral_hog",
            "wild_boar",

            // Base game - Characters
            "civ_alpine_scientist_female",
            "civ_alpine_scientist_male",
            "civ_scientist_female",
            "civ_scientist_male",
            "civ_alpine_worker_female_01",
            "civ_alpine_worker_male_01",
            "civ_node_hacker_male_01",
            "civ_node_hacker_male_02",
            "civ_node_hacker_male_03",
            "civ_node_hacker_male_04",
            "civ_techengineer_female",
            "civ_techengineer_male",
            "civ_miner_male",
            "civ_construction_worker_female_01",
            "civ_construction_worker_male_01",
            "civ_lumberyard_female_01",
            "civ_lumberyard_male_01",
            "civ_mechanic_female_01",
            "civ_mechanic_male_01",
            "civ_steelworker_female_01",
            "civ_steelworker_male_01",
            "civ_evil_target_female_01",
            "civ_evil_target_male_01",
            "civ_garland_crew_female_01",
            "civ_garland_crew_male_01",
            "civ_garland_location_scout_001",
            "civ_garland_pa_001",
            "civ_garland_pa_male_01",
            "civ_garland_stuntperson_helmet_female_01",
            "civ_garland_stuntperson_helmet_male_01",
            "civ_javi_soldier_001",
            "civ_javi_soldier_full_body",
            "civ_sargento_soldier_female_01",
            "civ_sargento_soldier_male_01",
            "civ_prisoner_female",
            "civ_prisoner_male",
            "civ_bolo_santosi_01",
            "civ_brrd_male_01",
            "civ_greenman_female_01",
            "civ_greenman_male_01",
            "civ_rome_male_01",
            "civ_beachgoer_female_01",
            "civ_beachgoer_male_01",
            "civ_dockworker_female_01",
            "civ_dockworker_male_01",
            "civ_favela_female_01",
            "civ_favela_male_01",
            "civ_rainforest_female_01",
            "civ_rainforest_male_01",
            "civ_desert_female_01",
            "civ_desert_male_01",
            "civ_grasslands_female_01",
            "civ_grasslands_male_01",
            "civ_business_female_001",
            "civ_business_male_001",
            "civ_vagrant_female_001",
            "civ_vagrant_male_001",
            "civ_athletic_female_001",
            "civ_athletic_male_001",
            "civ_soccer_aa_female_01",
            "civ_soccer_aa_male_01",
            "civ_soccer_esp_female_01",
            "civ_soccer_esp_male_01",
            "civ_soccer_female_001",
            "civ_soccer_male_001",
            "civ_upperclass_female_01",
            "civ_upperclass_male_01",
            "civ_farmer_female_01",
            "civ_farmer_male_01",
            "civ_bartender_female_01",
            "civ_bartender_male_01",
            "civ_airport_worker_female_01",
            "civ_airport_worker_male_01",
            "civ_art_vendor_female_01",
            "civ_art_vendor_male_01",
            "civ_factory_worker_female_01",
            "civ_factory_worker_male_01",
            "civ_food_vendor_female_01",
            "civ_food_vendor_male_01",
            "civ_gas_station_female_01",
            "civ_gas_station_male_01",
            "civ_graffitiartist_female_01",
            "civ_graffitiartist_male_01",
            "civ_street_musician_female_01",
            "civ_street_musician_male_01",

            // Base game - Rebels
            "sargentos_rebel_female_01",
            "sargentos_rebel_female_02",
            "sargentos_rebel_female_03",
            "sargentos_rebel_male_01",
            "sargentos_rebel_male_02",
            "sargentos_rebel_male_03",
            "female_rebel_001",
            "female_rebel_002",
            "female_rebel_003",
            "female_rebel_004",
            "female_rebel_prisoner_01",
            "female_rebel_prisoner_02",
            "female_rebel_prisoner_03",
            "female_rebel_tier_1_01",
            "female_rebel_tier_1_02",
            "female_rebel_tier_1_03",
            "female_rebel_tier_2_01",
            "female_rebel_tier_2_02",
            "female_rebel_tier_2_03",
            "female_rebel_tier_3_01",
            "female_rebel_tier_3_02",
            "female_rebel_tier_3_03",
            "male_rebel_001",
            "male_rebel_002",
            "male_rebel_003",
            "male_rebel_004",
            "male_rebel_prisoner_01",
            "male_rebel_prisoner_02",
            "male_rebel_prisoner_03",
            "male_rebel_tier_1_01",
            "male_rebel_tier_1_02",
            "male_rebel_tier_1_03",
            "male_rebel_tier_1_04",
            "male_rebel_tier_2_01",
            "male_rebel_tier_2_02",
            "male_rebel_tier_2_03",
            "male_rebel_tier_2_04",
            "male_rebel_tier_3_01",
            "male_rebel_tier_3_02",
            "male_rebel_tier_3_03",
            "male_rebel_tier_3_04",

            // Base game - Main characters
            "civ_female_aha_dancer_follower",
            "cesar",
            "dictator",
            "dictator_guard",
            "gabriela",
            "garland",
            "izzy",
            "javi",
            "lanza",
            "miguel_rodriguez",
            "mira",
            "oscar",
            "oscar_young",
            "sargento",
            "sheldon",
        };

        std::vector<std::string> result;
        std::copy_if(hints.begin(), hints.end(), std::back_inserter(result),
                     [&](const std::string &item) { return item.find(arguments) != std::string::npos; });

        return result;
    }
};
