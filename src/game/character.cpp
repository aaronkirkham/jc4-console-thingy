#include "character.h"

#include "addresses.h"
#include "entity_provider.h"
#include "player_manager.h"
#include "skin_change_req_handler.h"
#include "spawn_system.h"

#include "hashlittle.h"

#include <meow_hook/util.h>

namespace jc
{
// runtime container hashes
static const uint32_t HASH_CLASS_HASH   = "_class_hash"_hash_little;
static const uint32_t HASH_CCHARACTER   = "CCharacter"_hash_little;
static const uint32_t HASH_SKELETON     = "skeleton"_hash_little;
static const uint32_t HASH_CSKINSWAPPER = "CSkinSwapper"_hash_little;

// skeleton hashes
static const uint32_t HASH_SKEL_RICO      = "animations/skeletons/characters/rico.bsk"_hash_little;
static const uint32_t HASH_SKEL_WORLD     = "animations/skeletons/characters/world_sim.bsk"_hash_little;
static const uint32_t HASH_SKEL_COMBATANT = "animations/skeletons/characters/combatant.bsk"_hash_little;
static const uint32_t HASH_SKEL_TITAN     = "animations/skeletons/characters/titan.bsk"_hash_little;

CSkinSwapper* GetSkinSwapper(CCharacter* character)
{
    auto children = *(std::vector<std::shared_ptr<CGameObject>>*)((char*)character + 0x180);
    for (auto child : children) {
        if (child && *child->GetClassHash() == HASH_CSKINSWAPPER) {
            return (CSkinSwapper*)child.get();
        }
    }

    return nullptr;
}

static void ChangeSkinImpl(CCharacter* character, const CRuntimeContainer* container)
{
    auto                skin_swapper  = GetSkinSwapper(character);
    uint32_t            skeleton_hash = 0;
    SPartialModelState* model_state   = &character->m_modelState;
    SPartialModelData   model_data{};

    // get model data from runtime container
    CRuntimeContainer rc{container->m_base};
    for (auto cont_ptr = container->begin(); cont_ptr != container->end(); ++cont_ptr) {
        rc.m_container = cont_ptr;

        if (rc.GetHash(HASH_CLASS_HASH) == HASH_CCHARACTER) {
            skeleton_hash = rc.GetHash(HASH_SKELETON);

            // update the skin swapper instance, this will fix problems with changing rico skin via the main menu
            if (skin_swapper) {
                if (skin_swapper->m_currentSkin == skin_swapper->m_defaultSkin) {
                    for (decltype(model_state->m_modelCount) i = 0; i < model_state->m_modelCount; ++i) {
                        model_state->m_slot[i].m_modelInstance->m_flags &= 0xFFFFFFEEu;
                    }

                    // copy old model info
                    static CMatrix4f identity{};
                    meow_hook::func_call<void>(GetAddress(CHARACTER_MODEL_COPY_PARTS), &skin_swapper->m_oldModelState,
                                               model_state, &identity,
                                               &character->m_animatedModel.m_animationController->m_skinningPalette);
                }

                // @NOTE: can be anything just not the default skin hash.
                skin_swapper->m_requestedSkin = 0xDEADBEEF;
                skin_swapper->m_currentSkin   = 0xDEADBEEF;
            }

            // parse runtime container to get model data
            meow_hook::func_call<void>(GetAddress(CHARACTER_MODEL_SET_PROPERTIES), &model_data, &rc);

            // rebuild model parts
            meow_hook::func_call<void>(GetAddress(CHARACTER_MODEL_REBUILD_MODEL), model_state, &model_data,
                                       &character->m_worldTransform,
                                       &character->m_animatedModel.m_animationController->m_skinningPalette);

            break;
        }
    }

    if (!skeleton_hash) {
        return;
    }

    // clear skeleton lookup
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

                if (vtable == GetAddress(VTABLE_RENDERBLOCKCHARACTER)
                    || vtable == GetAddress(VTABLE_RENDERBLOCKCHARACTERSKIN)
                    || vtable == GetAddress(VTABLE_RENDERBLOCKGENERAL)) {
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
