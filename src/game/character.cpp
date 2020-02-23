#include "character.h"

#include "entity_provider.h"
#include "player_manager.h"
#include "skin_change_req_handler.h"
#include "spawn_system.h"

#include "hashlittle.h"

namespace jc
{
// runtime container hashes
static const uint32_t HASH_CLASS_HASH = "_class_hash"_hash_little;
static const uint32_t HASH_CCHARACTER = "CCharacter"_hash_little;
static const uint32_t HASH_MODEL      = "model"_hash_little;
static const uint32_t HASH_SKELETON   = "skeleton"_hash_little;
static const uint32_t HASH_FILEPATH   = "filepath"_hash_little;
static const uint32_t HASH_WEIGHT     = "Weight"_hash_little;

// skeleton hashes
static const uint32_t HASH_SKEL_RICO      = "animations/skeletons/characters/rico.bsk"_hash_little;
static const uint32_t HASH_SKEL_WORLD     = "animations/skeletons/characters/world_sim.bsk"_hash_little;
static const uint32_t HASH_SKEL_COMBATANT = "animations/skeletons/characters/combatant.bsk"_hash_little;
static const uint32_t HASH_SKEL_TITAN     = "animations/skeletons/characters/titan.bsk"_hash_little;

static void ChangeSkinImpl(CCharacter* character, const CRuntimeContainer* container)
{
    SPartialModelState* model_state    = &character->m_modelState;
    CAnimatedModel*     animated_model = &character->m_animatedModel;

    if (!model_state || !animated_model) {
        return;
    }

    uint32_t                                             skeleton_hash = 0;
    std::vector<std::vector<std::pair<uint32_t, float>>> model_infos;
    std::vector<float>                                   model_infos_weight_sum;

    // get all model info from runtime container
    CRuntimeContainer rc{container->m_base};
    for (auto cont_ptr = container->begin(); cont_ptr != container->end(); ++cont_ptr) {
        rc.m_container = cont_ptr;

        if (rc.GetHash(HASH_CLASS_HASH) == HASH_CCHARACTER) {
            auto model_hash = rc.GetHash(HASH_MODEL);
            skeleton_hash   = rc.GetHash(HASH_SKELETON);

            // model uses parts!
            if (model_hash == 0) {
                // CGameObjectUtil::CountSubComponents
                int32_t count = hk::func_call<int32_t>(0x1402A6CA0, &rc, 0xC160E555);
                model_infos.resize(count);
                model_infos_weight_sum.resize(count);

                uint32_t current_info_slot = 0;

                CRuntimeContainer sub_rc{rc.m_base};
                for (auto a = rc.begin(); a != rc.end(); ++a) {
                    sub_rc.m_container = a;

                    // model container
                    if (sub_rc.GetHash(HASH_CLASS_HASH) == 0xC160E555) {
                        CRuntimeContainer sub_sub_rc{sub_rc.m_base};

                        float weight_sum = 0.0f;

                        for (auto b = sub_rc.begin(); b != sub_rc.end(); ++b) {
                            sub_sub_rc.m_container = b;

                            // model entry
                            if (sub_sub_rc.GetHash(HASH_CLASS_HASH) == 0xAA09A7DC) {
                                model_hash   = sub_sub_rc.GetHash(HASH_FILEPATH);
                                float weight = sub_sub_rc.GetFloat(HASH_WEIGHT);

                                model_infos[current_info_slot].push_back({model_hash, weight});
                                weight_sum += weight;
                            }
                        }

                        model_infos_weight_sum[current_info_slot] = weight_sum;
                        ++current_info_slot;
                    }
                }
            } else {
                model_infos.resize(1);
                model_infos[0].push_back({model_hash, 0.0f});
            }

            break;
        }
    }

    // no model to set, don't continue or we might have weird issues with skeleton.
    if (model_infos.empty()) {
#ifdef _DEBUG
        __debugbreak();
#endif
        return;
    }

    // set model data from model info
    model_state->Reset();
    // for (const auto& info : model_infos) {
    for (size_t i = 0; i < model_infos.size(); ++i) {
        const auto& info = model_infos[i];
        if (info.empty()) {
            continue;
        }

        uint32_t model_hash = info[0].first;

        // if we have multiple models, fetch a random one!
        if (info.size() > 1) {
            // rotate random seed
            uint32_t random_seed    = *(uint32_t*)0x142C73A98;
            random_seed             = 214013 * random_seed + 2531011;
            *(uint32_t*)0x142C73A98 = random_seed;

            // generate a random number
            auto rng       = (((random_seed >> 16) | 0x3f8000) << 8);
            auto rng_float = *((float*)&rng);

            // choose a random model from the slots vector
            auto rng_weight = (rng_float - 1.0f) * model_infos_weight_sum[i];
            for (const auto& slot_info : info) {
                rng_weight = (rng_weight - slot_info.second);
                if (rng_weight <= 0.0f) {
                    model_hash = slot_info.first;
                    break;
                }
            }
        }

        model_state->SetModel(model_state->m_modelCount, model_hash, character->m_worldTransform,
                              &animated_model->m_animationController->m_skinningPalette);
    }

    // @TODO: fix CSkinSwapper copying model parts for ^ this (problematic when switching rico skin within the main menu
    // as it will copy the current model we just set, and the skeleton map will be bad after we change back)

    // @TODO: we want to do this, but not before the new skeleton is mapped,
    //		  otherwise we get an ugly flash of the model in T pose.
    SkeletonLookup::Get()->Empty();

    // only remap ped-character related skeletons
    if (skeleton_hash == HASH_SKEL_WORLD || skeleton_hash == HASH_SKEL_COMBATANT || skeleton_hash == HASH_SKEL_TITAN) {
        for (decltype(model_state->m_modelCount) i = 0; i < model_state->m_modelCount; ++i) {
            auto instance = model_state->m_slot[i].m_modelInstance;
            auto model    = instance->m_model.m_handle->m_ptr;

            // RBI info is unique to the model instance.
            // Keep track of if so we know when to apply the patched skeleton lookup
            SkeletonLookup::Get()->SetRBIInfo(&instance->m_rbiInfo);

            CModelRenderBlock** render_blocks      = model->m_renderBlocks;
            auto                render_block_count = model->m_totalRenderBlockCount;
            for (decltype(render_block_count) x = 0; x < render_block_count; ++x) {
                CModelRenderBlock* render_block = render_blocks[x];
                uint64_t           vtable       = *(uint64_t*)render_block;

                // RenderBlockCharacter & RenderBlockCharacterSkin & RenderBlockGeneral
                if (vtable == 0x141eac550 || vtable == 0x141eac3a0 || vtable == 0x141ebdf50) {
                    SkeletonLookup::Get()->Make(render_block);
                } else {
#ifdef _DEBUG
                    __debugbreak();
#endif
                }
            }
        }
    }
#ifdef _DEBUG
    else if (skeleton_hash != HASH_SKEL_RICO) {
        __debugbreak();
    }
#endif
}

void CCharacter::ChangeSkin(CSharedString& model_name, std::function<void()> callback)
{
    SkinChangeRequestHandler::Get()->Request(model_name, [this, callback](const CRuntimeContainer* container) {
        ChangeSkinImpl(this, container);
        if (callback) {
            callback();
        }
    });
}
}; // namespace jc
