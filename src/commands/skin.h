#pragma once

#include "command.h"

class SkinCommand : public ICommand
{
  public:
    virtual const char* GetCommand() override
    {
        return "skin";
    }

    virtual std::pair<bool, std::string> Handler(const std::string& arguments) override
    {
        std::vector<jc::CSpawnSystem::SResourceDef*, jc::allocator<jc::CSpawnSystem::SResourceDef*>> resources;
        if (jc::CSpawnSystem::instance().GetMatchingResources(arguments, &resources)) {
            jc::CPlayerManager::instance().GetLocalPlayerCharacter()->ChangeSkin(resources[0]->m_resourcePath);
            return {true, ""};
        }

        return {false, ""};
    }

    virtual std::vector<std::string> GetHints(const std::string& arguments) override
    {
        static std::array hints{
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
            "editor/entities/characters/main_characters/dictator.ee",
            "editor/entities/characters/main_characters/dictator_guard.ee",
            "gabriela",
            "garland",
            "izzy",
            "javi",
            "lanza",
            "editor/entities/characters/main_characters/miguel_rodriguez.ee",
            "mira",
            "oscar",
            "editor/entities/characters/main_characters/oscar_young.ee",
            "editor/entities/characters/main_characters/rico.ee",
            "sargento",
            "sheldon",

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
        };

        std::vector<std::string> result;
        std::copy_if(hints.begin(), hints.end(), std::back_inserter(result),
                     [&](const std::string& item) { return item.find(arguments) != std::string::npos; });

        return result;
    }
};
