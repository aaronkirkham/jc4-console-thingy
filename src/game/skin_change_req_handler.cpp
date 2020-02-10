#include "skin_change_req_handler.h"

#include "allocator.h"
#include "character.h"
#include "spawn_system.h"

#include "entity_provider.h"
#include "player_manager.h"

#include <algorithm>
#include <array>

namespace jc
{
static uintptr_t *DrawSkinBatches_orig = nullptr;

SkinChangeRequestHandler::SkinChangeRequestHandler()
{
    static std::once_flag _once;
    std::call_once(_once, [&] {
        m_provider = (CEntityProvider *)jc::_alloc(sizeof(CEntityProvider));
        hk::func_call<void>(0x14025F860, m_provider); // CEntityProvider::CEntityProvider

        // setup
        m_provider->m_resourceCache    = CSpawnSystem::instance().m_resourceCache;
        m_provider->m_priority         = 1; // high
        m_provider->m_streamerPriority = 4; // very high

        // NGraphicsEngine::CRenderBlockCharacter::DrawSkinBatches
        DrawSkinBatches_orig = hk::detour_func(0x14A36B420, DrawSkinBatches);
    });
}

void SkinChangeRequestHandler::Request(CSharedString &resource_path, std::function<void(const CRuntimeContainer *)> fn)
{
    // @TODO: some kind of cancel if we already requested something?
    //		  will there ever be a situation where another skin is requested before
    //		  the current one is spawned?

    m_callback = fn;

    // CEntityProvider::LoadResources
    hk::func_call<void>(0x140284870, m_provider, &resource_path);
}

void SkinChangeRequestHandler::Update()
{
#if 0
    constexpr std::array available_models = {
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

#if 1
    static auto last_change = std::chrono::system_clock::now() + std::chrono::seconds(10);
    static auto next_id     = -1;

    auto character = jc::CPlayerManager::GetLocalPlayerCharacter();

    if (character && last_change < std::chrono::system_clock::now()) {
        last_change = std::chrono::system_clock::now() + std::chrono::seconds(5);
        std::vector<jc::CSpawnSystem::SResourceDef *, jc::allocator<jc::CSpawnSystem::SResourceDef *>> resources;

        next_id++;
        if (next_id >= available_models.size()) {
            next_id = 0;
        }

        OutputDebugStringA(available_models[next_id]);
        OutputDebugStringA("\n");

        if (jc::CSpawnSystem::instance().GetMatchingResources(available_models[next_id], &resources)) {
            assert(!resources.empty());

            character->ChangeSkin(resources[0]->m_resourcePath, [] {
                last_change = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
            });
        } else {
            OutputDebugStringA(" - NO MATCHING RESOURCES.\n");
        }
    }
#endif
#endif

    // CEntityProvider::UpdateInternal
    hk::func_call<void>(
        0x140294F20, m_provider, this,
        (entity_provider_callback_t)[](void *puser) {
            auto request_handler = (SkinChangeRequestHandler *)puser;
            assert(request_handler->m_provider->m_entityResourceLoader);
            request_handler->m_callback((jc::CRuntimeContainer *)&request_handler->m_provider->m_entityResourceLoader
                                            ->m_propertyContainer->m_base);
        },
        (entity_provider_callback_t)[](void *puser) {
#ifdef _DEBUG
            // spawn failed!
            __debugbreak();
#endif
        });
}

// @NOTE: Because all the models are cached and shared, they will also use the same skeleton lookup which will cause
//		  visual artifacts with random peds. Because the rbi_info struct is unique to each CModelInstance, we can
//        switch the skeleton lookup before the skin batches are drawn, and switch back to the original after to
//        prevent this from happening. magic.
void SkinChangeRequestHandler::DrawSkinBatches(jc::CModelRenderBlock *render_block, void *render_context,
                                               void *rbi_info, bool unknown)
{
    int16_t *original_skeleton_lookup = nullptr;

    {
        std::lock_guard<std::mutex> lk{jc::SkeletonLookup::Get()->m_mutex};

        const auto inst               = &jc::SkeletonLookup::Get()->m_rbiInstances;
        const bool needs_skeleton_map = std::find(inst->begin(), inst->end(), rbi_info) != inst->end();

        // set the custom skeleton lookup before render
        if (needs_skeleton_map) {
            const auto lookup = &jc::SkeletonLookup::Get()->m_skeletonLookup;
            const auto it     = lookup->find(render_block);

            if (it != lookup->end() && (*it).second != nullptr) {
                original_skeleton_lookup               = render_block->m_mesh->m_skeletonLookup;
                render_block->m_mesh->m_skeletonLookup = (*it).second;
            }
        }
    }

    ((decltype(DrawSkinBatches) *)(DrawSkinBatches_orig))(render_block, render_context, rbi_info, unknown);

    // restore the original skeleton lookup after render
    if (original_skeleton_lookup) {
        render_block->m_mesh->m_skeletonLookup = original_skeleton_lookup;
    }
}

static std::array kBoneMappingsRico = {
    std::pair<uint32_t, uint32_t>{2394094972, 0},   std::pair<uint32_t, uint32_t>{1252689883, 1},
    std::pair<uint32_t, uint32_t>{1489289568, 2},   std::pair<uint32_t, uint32_t>{4084831349, 3},
    std::pair<uint32_t, uint32_t>{3477960784, 4},   std::pair<uint32_t, uint32_t>{3223761349, 5},
    std::pair<uint32_t, uint32_t>{1555958995, 6},   std::pair<uint32_t, uint32_t>{1459912489, 7},
    std::pair<uint32_t, uint32_t>{2576446000, 8},   std::pair<uint32_t, uint32_t>{4129665182, 9},
    std::pair<uint32_t, uint32_t>{3366567460, 10},  std::pair<uint32_t, uint32_t>{1108521370, 11},
    std::pair<uint32_t, uint32_t>{3143544379, 12},  std::pair<uint32_t, uint32_t>{3685314612, 13},
    std::pair<uint32_t, uint32_t>{2900729280, 14},  std::pair<uint32_t, uint32_t>{1203626394, 15},
    std::pair<uint32_t, uint32_t>{1227774589, 16},  std::pair<uint32_t, uint32_t>{914656450, 17},
    std::pair<uint32_t, uint32_t>{2633462918, 18},  std::pair<uint32_t, uint32_t>{1748179293, 19},
    std::pair<uint32_t, uint32_t>{1141701893, 20},  std::pair<uint32_t, uint32_t>{2395612961, 21},
    std::pair<uint32_t, uint32_t>{519011590, 22},   std::pair<uint32_t, uint32_t>{3970182939, 23},
    std::pair<uint32_t, uint32_t>{994889083, 24},   std::pair<uint32_t, uint32_t>{4125003590, 25},
    std::pair<uint32_t, uint32_t>{3242100449, 26},  std::pair<uint32_t, uint32_t>{1108266938, 27},
    std::pair<uint32_t, uint32_t>{1926223301, 28},  std::pair<uint32_t, uint32_t>{661265071, 29},
    std::pair<uint32_t, uint32_t>{3066825915, 30},  std::pair<uint32_t, uint32_t>{912916643, 31},
    std::pair<uint32_t, uint32_t>{2317283342, 32},  std::pair<uint32_t, uint32_t>{3441638405, 33},
    std::pair<uint32_t, uint32_t>{2760225489, 34},  std::pair<uint32_t, uint32_t>{3964175071, 35},
    std::pair<uint32_t, uint32_t>{2752631666, 36},  std::pair<uint32_t, uint32_t>{100325420, 37},
    std::pair<uint32_t, uint32_t>{1491560446, 38},  std::pair<uint32_t, uint32_t>{516832345, 39},
    std::pair<uint32_t, uint32_t>{1252046159, 40},  std::pair<uint32_t, uint32_t>{4033932099, 41},
    std::pair<uint32_t, uint32_t>{2639211572, 42},  std::pair<uint32_t, uint32_t>{1233549747, 43},
    std::pair<uint32_t, uint32_t>{2224513931, 44},  std::pair<uint32_t, uint32_t>{2758980529, 45},
    std::pair<uint32_t, uint32_t>{2695123779, 46},  std::pair<uint32_t, uint32_t>{2965608343, 47},
    std::pair<uint32_t, uint32_t>{2193546120, 48},  std::pair<uint32_t, uint32_t>{2860814395, 49},
    std::pair<uint32_t, uint32_t>{1308091062, 50},  std::pair<uint32_t, uint32_t>{1470279966, 51},
    std::pair<uint32_t, uint32_t>{3360101082, 52},  std::pair<uint32_t, uint32_t>{2438260305, 53},
    std::pair<uint32_t, uint32_t>{2923024671, 54},  std::pair<uint32_t, uint32_t>{455640735, 55},
    std::pair<uint32_t, uint32_t>{3081454700, 56},  std::pair<uint32_t, uint32_t>{2580095936, 57},
    std::pair<uint32_t, uint32_t>{2796181772, 58},  std::pair<uint32_t, uint32_t>{3959365138, 59},
    std::pair<uint32_t, uint32_t>{1858091382, 60},  std::pair<uint32_t, uint32_t>{1674505706, 61},
    std::pair<uint32_t, uint32_t>{3404332141, 62},  std::pair<uint32_t, uint32_t>{175240702, 63},
    std::pair<uint32_t, uint32_t>{3536936145, 64},  std::pair<uint32_t, uint32_t>{3768499, 65},
    std::pair<uint32_t, uint32_t>{3179998832, 66},  std::pair<uint32_t, uint32_t>{4120908208, 67},
    std::pair<uint32_t, uint32_t>{1413498797, 68},  std::pair<uint32_t, uint32_t>{3403576838, 69},
    std::pair<uint32_t, uint32_t>{2550461854, 70},  std::pair<uint32_t, uint32_t>{3416507533, 71},
    std::pair<uint32_t, uint32_t>{1518475483, 72},  std::pair<uint32_t, uint32_t>{165729162, 73},
    std::pair<uint32_t, uint32_t>{3267328078, 74},  std::pair<uint32_t, uint32_t>{1565745466, 75},
    std::pair<uint32_t, uint32_t>{973873416, 76},   std::pair<uint32_t, uint32_t>{2222924337, 77},
    std::pair<uint32_t, uint32_t>{3915943525, 78},  std::pair<uint32_t, uint32_t>{3794447476, 79},
    std::pair<uint32_t, uint32_t>{724613817, 80},   std::pair<uint32_t, uint32_t>{1425464583, 81},
    std::pair<uint32_t, uint32_t>{1467550696, 82},  std::pair<uint32_t, uint32_t>{2190797639, 83},
    std::pair<uint32_t, uint32_t>{586670052, 84},   std::pair<uint32_t, uint32_t>{3370773187, 85},
    std::pair<uint32_t, uint32_t>{2725811682, 86},  std::pair<uint32_t, uint32_t>{2544362859, 87},
    std::pair<uint32_t, uint32_t>{859145367, 88},   std::pair<uint32_t, uint32_t>{529012006, 89},
    std::pair<uint32_t, uint32_t>{3908814753, 90},  std::pair<uint32_t, uint32_t>{1592734258, 91},
    std::pair<uint32_t, uint32_t>{1187820986, 92},  std::pair<uint32_t, uint32_t>{2077283755, 93},
    std::pair<uint32_t, uint32_t>{1256173202, 94},  std::pair<uint32_t, uint32_t>{1654913162, 95},
    std::pair<uint32_t, uint32_t>{3451757196, 96},  std::pair<uint32_t, uint32_t>{3794315760, 97},
    std::pair<uint32_t, uint32_t>{2731679068, 98},  std::pair<uint32_t, uint32_t>{291412610, 99},
    std::pair<uint32_t, uint32_t>{3805473304, 100}, std::pair<uint32_t, uint32_t>{3897936212, 101},
    std::pair<uint32_t, uint32_t>{3023242332, 102}, std::pair<uint32_t, uint32_t>{560388075, 103},
    std::pair<uint32_t, uint32_t>{1348953638, 104}, std::pair<uint32_t, uint32_t>{3689970757, 105},
    std::pair<uint32_t, uint32_t>{405273388, 106},  std::pair<uint32_t, uint32_t>{3920139427, 107},
    std::pair<uint32_t, uint32_t>{3874020345, 108}, std::pair<uint32_t, uint32_t>{1186717741, 109},
    std::pair<uint32_t, uint32_t>{4052933939, 110}, std::pair<uint32_t, uint32_t>{2858741845, 111},
    std::pair<uint32_t, uint32_t>{4137047442, 112}, std::pair<uint32_t, uint32_t>{3076404574, 113},
    std::pair<uint32_t, uint32_t>{33194846, 114},   std::pair<uint32_t, uint32_t>{1565985537, 115},
    std::pair<uint32_t, uint32_t>{552428061, 116},  std::pair<uint32_t, uint32_t>{2334448475, 117},
    std::pair<uint32_t, uint32_t>{196868699, 118},  std::pair<uint32_t, uint32_t>{3041309387, 119},
    std::pair<uint32_t, uint32_t>{1017860219, 120}, std::pair<uint32_t, uint32_t>{2055766129, 121},
    std::pair<uint32_t, uint32_t>{3159852471, 122}, std::pair<uint32_t, uint32_t>{3473275293, 123},
    std::pair<uint32_t, uint32_t>{1513462622, 124}, std::pair<uint32_t, uint32_t>{575268596, 125},
    std::pair<uint32_t, uint32_t>{2392783451, 126}, std::pair<uint32_t, uint32_t>{3658374521, 127},
    std::pair<uint32_t, uint32_t>{1621410674, 128}, std::pair<uint32_t, uint32_t>{567513068, 129},
    std::pair<uint32_t, uint32_t>{3410833543, 130}, std::pair<uint32_t, uint32_t>{740730292, 131},
    std::pair<uint32_t, uint32_t>{1831608258, 132}, std::pair<uint32_t, uint32_t>{2543190806, 133},
    std::pair<uint32_t, uint32_t>{1320882368, 134}, std::pair<uint32_t, uint32_t>{1129663262, 135},
    std::pair<uint32_t, uint32_t>{1083769543, 136}, std::pair<uint32_t, uint32_t>{1770750798, 137},
    std::pair<uint32_t, uint32_t>{1307941776, 138}, std::pair<uint32_t, uint32_t>{3690742995, 139},
    std::pair<uint32_t, uint32_t>{1668646018, 140}, std::pair<uint32_t, uint32_t>{3943696337, 141},
    std::pair<uint32_t, uint32_t>{4175912225, 142}, std::pair<uint32_t, uint32_t>{967579856, 143},
    std::pair<uint32_t, uint32_t>{956949182, 144},  std::pair<uint32_t, uint32_t>{2453029768, 145},
    std::pair<uint32_t, uint32_t>{380016446, 146},  std::pair<uint32_t, uint32_t>{4221697845, 147},
    std::pair<uint32_t, uint32_t>{1649197646, 148}, std::pair<uint32_t, uint32_t>{1125105860, 149},
    std::pair<uint32_t, uint32_t>{2111592436, 150}, std::pair<uint32_t, uint32_t>{342908061, 151},
    std::pair<uint32_t, uint32_t>{3803182815, 152}, std::pair<uint32_t, uint32_t>{3130931077, 153},
    std::pair<uint32_t, uint32_t>{3995526818, 154}, std::pair<uint32_t, uint32_t>{3909468794, 155},
    std::pair<uint32_t, uint32_t>{3905299996, 156}, std::pair<uint32_t, uint32_t>{802760491, 157},
    std::pair<uint32_t, uint32_t>{1410227229, 158}, std::pair<uint32_t, uint32_t>{2715072942, 159},
    std::pair<uint32_t, uint32_t>{2420269632, 160}, std::pair<uint32_t, uint32_t>{4197176223, 161},
    std::pair<uint32_t, uint32_t>{3868652464, 162}, std::pair<uint32_t, uint32_t>{264702988, 163},
    std::pair<uint32_t, uint32_t>{1954358957, 164}, std::pair<uint32_t, uint32_t>{54318578, 165},
    std::pair<uint32_t, uint32_t>{574463303, 166},  std::pair<uint32_t, uint32_t>{1390825376, 167},
    std::pair<uint32_t, uint32_t>{1854359826, 168}, std::pair<uint32_t, uint32_t>{1459217846, 169},
    std::pair<uint32_t, uint32_t>{663512083, 170},  std::pair<uint32_t, uint32_t>{2445592106, 171},
    std::pair<uint32_t, uint32_t>{4286418273, 172}, std::pair<uint32_t, uint32_t>{4118819487, 173},
    std::pair<uint32_t, uint32_t>{578106827, 174},  std::pair<uint32_t, uint32_t>{761738650, 175},
    std::pair<uint32_t, uint32_t>{3479324205, 176}, std::pair<uint32_t, uint32_t>{3471724469, 177},
    std::pair<uint32_t, uint32_t>{2687883648, 178}, std::pair<uint32_t, uint32_t>{1140707517, 179},
    std::pair<uint32_t, uint32_t>{2144407270, 180}, std::pair<uint32_t, uint32_t>{1019608705, 181},
    std::pair<uint32_t, uint32_t>{793140648, 182},  std::pair<uint32_t, uint32_t>{1088841951, 183},
    std::pair<uint32_t, uint32_t>{2666929011, 184}, std::pair<uint32_t, uint32_t>{2791220655, 185},
    std::pair<uint32_t, uint32_t>{3251617740, 186}, std::pair<uint32_t, uint32_t>{1998472915, 187},
    std::pair<uint32_t, uint32_t>{843322903, 188},  std::pair<uint32_t, uint32_t>{2915928333, 189},
    std::pair<uint32_t, uint32_t>{1126173219, 190}, std::pair<uint32_t, uint32_t>{2127212215, 191},
    std::pair<uint32_t, uint32_t>{1981159028, 192}, std::pair<uint32_t, uint32_t>{4044845400, 193},
    std::pair<uint32_t, uint32_t>{3053108164, 194}, std::pair<uint32_t, uint32_t>{1194293737, 195},
    std::pair<uint32_t, uint32_t>{1880966197, 196}, std::pair<uint32_t, uint32_t>{553268143, 197},
    std::pair<uint32_t, uint32_t>{921735734, 198},  std::pair<uint32_t, uint32_t>{103199345, 199},
    std::pair<uint32_t, uint32_t>{3781048348, 200}, std::pair<uint32_t, uint32_t>{722113249, 201},
    std::pair<uint32_t, uint32_t>{426185881, 202},  std::pair<uint32_t, uint32_t>{1646043647, 203},
    std::pair<uint32_t, uint32_t>{1530280872, 204}, std::pair<uint32_t, uint32_t>{3807281547, 205},
    std::pair<uint32_t, uint32_t>{3136087302, 206}, std::pair<uint32_t, uint32_t>{165669168, 207},
    std::pair<uint32_t, uint32_t>{1655909510, 208}, std::pair<uint32_t, uint32_t>{1984075092, 209},
    std::pair<uint32_t, uint32_t>{2211922494, 210}, std::pair<uint32_t, uint32_t>{2289082043, 211},
    std::pair<uint32_t, uint32_t>{747680433, 212},  std::pair<uint32_t, uint32_t>{1221183753, 213},
    std::pair<uint32_t, uint32_t>{3915181747, 214}, std::pair<uint32_t, uint32_t>{1741560029, 215},
    std::pair<uint32_t, uint32_t>{2073368859, 216}, std::pair<uint32_t, uint32_t>{709778699, 217},
    std::pair<uint32_t, uint32_t>{2713945292, 218}, std::pair<uint32_t, uint32_t>{1087663378, 219},
    std::pair<uint32_t, uint32_t>{2741324850, 220}, std::pair<uint32_t, uint32_t>{2619108735, 221},
    std::pair<uint32_t, uint32_t>{1063276332, 222}, std::pair<uint32_t, uint32_t>{2737431107, 223},
    std::pair<uint32_t, uint32_t>{870802327, 224},  std::pair<uint32_t, uint32_t>{2160032645, 225},
    std::pair<uint32_t, uint32_t>{3329358717, 226}, std::pair<uint32_t, uint32_t>{2836032339, 227},
    std::pair<uint32_t, uint32_t>{4223045062, 228}, std::pair<uint32_t, uint32_t>{485621676, 229},
    std::pair<uint32_t, uint32_t>{863510078, 230},  std::pair<uint32_t, uint32_t>{3641685623, 231},
    std::pair<uint32_t, uint32_t>{37904559, 232},   std::pair<uint32_t, uint32_t>{3443459293, 233},
    std::pair<uint32_t, uint32_t>{2975848301, 234}, std::pair<uint32_t, uint32_t>{3645907746, 235},
    std::pair<uint32_t, uint32_t>{2112223537, 236}, std::pair<uint32_t, uint32_t>{578833067, 237},
    std::pair<uint32_t, uint32_t>{2576307509, 238}, std::pair<uint32_t, uint32_t>{4095443743, 239},
    std::pair<uint32_t, uint32_t>{2594570093, 240}, std::pair<uint32_t, uint32_t>{3615785407, 241},
    std::pair<uint32_t, uint32_t>{766926689, 242},  std::pair<uint32_t, uint32_t>{661848656, 243},
    std::pair<uint32_t, uint32_t>{2011225857, 244}, std::pair<uint32_t, uint32_t>{1514792347, 245},
    std::pair<uint32_t, uint32_t>{1474535392, 246}, std::pair<uint32_t, uint32_t>{1180683473, 247},
    std::pair<uint32_t, uint32_t>{1567288207, 248}, std::pair<uint32_t, uint32_t>{1297945330, 249},
    std::pair<uint32_t, uint32_t>{2438248688, 250}, std::pair<uint32_t, uint32_t>{2513859035, 251},
    std::pair<uint32_t, uint32_t>{2046502109, 252}, std::pair<uint32_t, uint32_t>{3473331380, 253},
    std::pair<uint32_t, uint32_t>{297408201, 254},  std::pair<uint32_t, uint32_t>{102891694, 255},
    std::pair<uint32_t, uint32_t>{3217444895, 256}, std::pair<uint32_t, uint32_t>{1059203173, 257},
    std::pair<uint32_t, uint32_t>{2636662382, 258}, std::pair<uint32_t, uint32_t>{288226583, 259},
    std::pair<uint32_t, uint32_t>{2139603822, 260}, std::pair<uint32_t, uint32_t>{2310005779, 261},
    std::pair<uint32_t, uint32_t>{3914499528, 262}, std::pair<uint32_t, uint32_t>{174621028, 263},
    std::pair<uint32_t, uint32_t>{4025642426, 264}, std::pair<uint32_t, uint32_t>{3563732574, 265},
    std::pair<uint32_t, uint32_t>{2413216009, 266}, std::pair<uint32_t, uint32_t>{2733242987, 267},
    std::pair<uint32_t, uint32_t>{2494648254, 268}, std::pair<uint32_t, uint32_t>{990827083, 269},
    std::pair<uint32_t, uint32_t>{3232932980, 270}, std::pair<uint32_t, uint32_t>{448731402, 271},
    std::pair<uint32_t, uint32_t>{3942931707, 272}, std::pair<uint32_t, uint32_t>{2956600658, 273},
    std::pair<uint32_t, uint32_t>{1495018146, 274}, std::pair<uint32_t, uint32_t>{20824016, 275},
    std::pair<uint32_t, uint32_t>{2224549021, 276}, std::pair<uint32_t, uint32_t>{866849826, 277},
    std::pair<uint32_t, uint32_t>{2620792770, 278}, std::pair<uint32_t, uint32_t>{3727732900, 279},
    std::pair<uint32_t, uint32_t>{3491236634, 280}, std::pair<uint32_t, uint32_t>{2271170787, 281},
    std::pair<uint32_t, uint32_t>{743077609, 282},  std::pair<uint32_t, uint32_t>{254324665, 283},
    std::pair<uint32_t, uint32_t>{1105106875, 284}, std::pair<uint32_t, uint32_t>{2616271571, 285},
};

static std::array kBoneMappingsWorldSim = {
    std::pair<uint32_t, uint32_t>{2394094972, 0},  std::pair<uint32_t, uint32_t>{1252689883, 1},
    std::pair<uint32_t, uint32_t>{1489289568, 2},  std::pair<uint32_t, uint32_t>{4084831349, 3},
    std::pair<uint32_t, uint32_t>{3477960784, 4},  std::pair<uint32_t, uint32_t>{3223761349, 5},
    std::pair<uint32_t, uint32_t>{1555958995, 6},  std::pair<uint32_t, uint32_t>{1459912489, 7},
    std::pair<uint32_t, uint32_t>{4129665182, 8},  std::pair<uint32_t, uint32_t>{3366567460, 9},
    std::pair<uint32_t, uint32_t>{1108521370, 10}, std::pair<uint32_t, uint32_t>{1227774589, 11},
    std::pair<uint32_t, uint32_t>{1141701893, 12}, std::pair<uint32_t, uint32_t>{4125003590, 13},
    std::pair<uint32_t, uint32_t>{2451919769, 14}, std::pair<uint32_t, uint32_t>{330130356, 15},
    std::pair<uint32_t, uint32_t>{455640735, 16},  std::pair<uint32_t, uint32_t>{3370773187, 17},
    std::pair<uint32_t, uint32_t>{912916643, 18},  std::pair<uint32_t, uint32_t>{100325420, 19},
    std::pair<uint32_t, uint32_t>{4120908208, 20}, std::pair<uint32_t, uint32_t>{1491560446, 21},
    std::pair<uint32_t, uint32_t>{1413498797, 22}, std::pair<uint32_t, uint32_t>{2639211572, 23},
    std::pair<uint32_t, uint32_t>{1518475483, 24}, std::pair<uint32_t, uint32_t>{3023242332, 25},
    std::pair<uint32_t, uint32_t>{560388075, 26},  std::pair<uint32_t, uint32_t>{1348953638, 27},
    std::pair<uint32_t, uint32_t>{3689970757, 28}, std::pair<uint32_t, uint32_t>{3920139427, 29},
    std::pair<uint32_t, uint32_t>{3874020345, 30}, std::pair<uint32_t, uint32_t>{1186717741, 31},
    std::pair<uint32_t, uint32_t>{4052933939, 32}, std::pair<uint32_t, uint32_t>{2858741845, 33},
    std::pair<uint32_t, uint32_t>{4137047442, 34}, std::pair<uint32_t, uint32_t>{2334448475, 35},
    std::pair<uint32_t, uint32_t>{196868699, 36},  std::pair<uint32_t, uint32_t>{3041309387, 37},
    std::pair<uint32_t, uint32_t>{1513462622, 38}, std::pair<uint32_t, uint32_t>{575268596, 39},
    std::pair<uint32_t, uint32_t>{3658374521, 40}, std::pair<uint32_t, uint32_t>{567513068, 41},
    std::pair<uint32_t, uint32_t>{740730292, 42},  std::pair<uint32_t, uint32_t>{1831608258, 43},
    std::pair<uint32_t, uint32_t>{2453029768, 44}, std::pair<uint32_t, uint32_t>{380016446, 45},
    std::pair<uint32_t, uint32_t>{4221697845, 46}, std::pair<uint32_t, uint32_t>{1649197646, 47},
    std::pair<uint32_t, uint32_t>{2111592436, 48}, std::pair<uint32_t, uint32_t>{342908061, 49},
    std::pair<uint32_t, uint32_t>{3803182815, 50}, std::pair<uint32_t, uint32_t>{3130931077, 51},
    std::pair<uint32_t, uint32_t>{3995526818, 52}, std::pair<uint32_t, uint32_t>{3909468794, 53},
    std::pair<uint32_t, uint32_t>{2420269632, 54}, std::pair<uint32_t, uint32_t>{4197176223, 55},
    std::pair<uint32_t, uint32_t>{3868652464, 56}, std::pair<uint32_t, uint32_t>{1390825376, 57},
    std::pair<uint32_t, uint32_t>{1854359826, 58}, std::pair<uint32_t, uint32_t>{663512083, 59},
    std::pair<uint32_t, uint32_t>{2445592106, 60}, std::pair<uint32_t, uint32_t>{4118819487, 61},
    std::pair<uint32_t, uint32_t>{578106827, 62},  std::pair<uint32_t, uint32_t>{3053108164, 63},
    std::pair<uint32_t, uint32_t>{3645907746, 64}, std::pair<uint32_t, uint32_t>{2112223537, 65},
    std::pair<uint32_t, uint32_t>{578833067, 66},  std::pair<uint32_t, uint32_t>{2576307509, 67},
    std::pair<uint32_t, uint32_t>{4095443743, 68}, std::pair<uint32_t, uint32_t>{3615785407, 69},
    std::pair<uint32_t, uint32_t>{1180683473, 70}, std::pair<uint32_t, uint32_t>{2513859035, 71},
    std::pair<uint32_t, uint32_t>{2046502109, 72}, std::pair<uint32_t, uint32_t>{3473331380, 73},
    std::pair<uint32_t, uint32_t>{297408201, 74},  std::pair<uint32_t, uint32_t>{3217444895, 75},
    std::pair<uint32_t, uint32_t>{3563732574, 76}, std::pair<uint32_t, uint32_t>{3914499528, 77},
    std::pair<uint32_t, uint32_t>{2956600658, 78}, std::pair<uint32_t, uint32_t>{20824016, 79},
    std::pair<uint32_t, uint32_t>{2224549021, 80}, std::pair<uint32_t, uint32_t>{866849826, 81},
    std::pair<uint32_t, uint32_t>{3727732900, 82}, std::pair<uint32_t, uint32_t>{3491236634, 83},
    std::pair<uint32_t, uint32_t>{2271170787, 84}, std::pair<uint32_t, uint32_t>{743077609, 85},
};

void SkeletonLookup::Make(CModelRenderBlock *render_block)
{
    assert(render_block && render_block->m_mesh);

    auto skeleton_lookup = render_block->m_mesh->m_skeletonLookup;
    auto lookup_size     = render_block->m_mesh->m_skeletonLookupSize;

    assert(skeleton_lookup);
    assert(lookup_size > 0);

    // copy to new skeleton lookup
    auto mapped_skeleton_lookup = (int16_t *)jc::_alloc(lookup_size * sizeof(int16_t));
    assert(mapped_skeleton_lookup);
    memcpy(mapped_skeleton_lookup, skeleton_lookup, (lookup_size * sizeof(int16_t)));

    // remap the skeleton bone indices
    // Credits to @xforce for the original JC3MP implementation.
    for (int32_t n = 0; n < lookup_size; ++n) {
        auto source_index = mapped_skeleton_lookup[n];

        uint32_t source_hash = 0;
        for (auto &&world_sim_map : kBoneMappingsWorldSim) {
            if (world_sim_map.second == static_cast<uint32_t>(source_index)) {
                source_hash = world_sim_map.first;
                break;
            }
        }
        if (source_hash != 0) {
            for (auto &&rico_map : kBoneMappingsRico) {
                if (rico_map.first == source_hash) {
                    mapped_skeleton_lookup[n] = static_cast<int16_t>(rico_map.second);
                    break;
                }
            }
        } else {
            mapped_skeleton_lookup[n] = 0;
        }
    }

    std::lock_guard<std::mutex> lk{m_mutex};
    m_skeletonLookup[render_block] = mapped_skeleton_lookup;
}

void SkeletonLookup::Empty()
{
    std::lock_guard<std::mutex> lk{m_mutex};

    m_rbiInstances.clear();

    for (auto &&lookup : m_skeletonLookup) {
        jc::_free(lookup.second);
        lookup.second = nullptr;
    }

    m_skeletonLookup.clear();
}
}; // namespace jc
