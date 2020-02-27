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
static const uint32_t HASH_SKELETON   = "skeleton"_hash_little;

// skeleton hashes
static const uint32_t HASH_SKEL_RICO      = "animations/skeletons/characters/rico.bsk"_hash_little;
static const uint32_t HASH_SKEL_WORLD     = "animations/skeletons/characters/world_sim.bsk"_hash_little;
static const uint32_t HASH_SKEL_COMBATANT = "animations/skeletons/characters/combatant.bsk"_hash_little;
static const uint32_t HASH_SKEL_TITAN     = "animations/skeletons/characters/titan.bsk"_hash_little;

static void ChangeSkinImpl(CCharacter* character, const CRuntimeContainer* container)
{
    uint32_t            skeleton_hash = 0;
    SPartialModelState* model_state   = &character->m_modelState;
    SPartialModelData   model_data{};

    // @TODO: fix CSkinSwapper copying model parts from model_state (problematic when switching rico skin within
    // the main menu as it will copy the current model we just set, and the skeleton map will be bad after we
    // change back)

    // get model data from runtime container
    CRuntimeContainer rc{container->m_base};
    for (auto cont_ptr = container->begin(); cont_ptr != container->end(); ++cont_ptr) {
        rc.m_container = cont_ptr;

        if (rc.GetHash(HASH_CLASS_HASH) == HASH_CCHARACTER) {
            skeleton_hash = rc.GetHash(HASH_SKELETON);

            // parse runtime container to get model data
            hk::func_call<void>(0x14028C790, &model_data, &rc);

            // rebuild model parts
            hk::func_call<void>(0x147BFFE90, model_state, &model_data, &character->m_worldTransform,
                                &character->m_animatedModel.m_animationController->m_skinningPalette);

            break;
        }
    }

    // @TODO: we want to do this, but not before the new skeleton is mapped,
    //		  otherwise we get an ugly flash of the model in T pose.
    SkeletonLookup::Get()->Empty();

    // only remap ped-character related skeletons
    if (skeleton_hash == HASH_SKEL_WORLD || skeleton_hash == HASH_SKEL_COMBATANT || skeleton_hash == HASH_SKEL_TITAN) {
        for (decltype(model_state->m_modelCount) i = 0; i < model_state->m_modelCount; ++i) {
            auto instance = model_state->m_slot[i].m_modelInstance;
            auto model    = instance->m_model.m_handle->m_ptr;

            CModelRenderBlock** render_blocks      = model->m_renderBlocks;
            auto                render_block_count = model->m_totalRenderBlockCount;
            for (decltype(render_block_count) x = 0; x < render_block_count; ++x) {
                CModelRenderBlock* render_block = render_blocks[x];
                uint64_t           vtable       = *(uint64_t*)render_block;

                // RenderBlockCharacter & RenderBlockCharacterSkin & RenderBlockGeneral
                if (vtable == 0x141eac550 || vtable == 0x141eac3a0 || vtable == 0x141ebdf50) {
                    SkeletonLookup::Get()->Make(&instance->m_rbiInfo, render_block);
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
