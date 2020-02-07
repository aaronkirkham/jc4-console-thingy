#include "character.h"

#include "entity_provider.h"
#include "player_manager.h"
#include "skin_change_req_handler.h"
#include "spawn_system.h"

#include "hashlittle.h"

#ifdef DUMP_BONE_MAP_STUFF
#include <fstream>
static void DumpBoneMap(void* skin_swapper)
{
    auto          animated_model    = hk::func_call<void*>(0x1407D9F10, skin_swapper);
    auto          animation_control = *(void**)((char*)animated_model + 0xE8);
    std::ofstream stream("c:/users/aaron/desktop/bonemap.txt");
    int64_t       bone_count = hk::func_call<int64_t>(0x140244810, animation_control); // GetBoneCount
    for (decltype(bone_count) i = 0; i < bone_count; ++i) {
        auto name_hash = hk::func_call<uint32_t>(0x140244830, animation_control, i); // GetBoneIndexHashedName
        stream << "std::pair<uint32_t, uint32_t>{" << name_hash << ", " << i << "}," << std::endl;
    }
    stream.close();
}
#endif

namespace jc
{
static void ChangeSkinImpl(CCharacter* character, const CRuntimeContainer* container)
{
    static uint32_t _class_hash = "_class_hash"_hash_little;

    SPartialModelState* model_state    = &character->m_modelState;
    CAnimatedModel*     animated_model = &character->m_animatedModel;

    if (!model_state || !animated_model) {
        return;
    }

    uint32_t model_hash    = 0;
    uint32_t skeleton_hash = 0;

    CRuntimeContainer rc{container->m_base};
    for (auto cont_ptr = container->begin(); cont_ptr != container->end(); ++cont_ptr) {
        rc.m_container = cont_ptr;

        if (rc.GetHash(_class_hash) == "CCharacter"_hash_little) {
            model_hash    = rc.GetHash("model"_hash_little);
            skeleton_hash = rc.GetHash("skeleton"_hash_little);

            // probably using parts..
            if (model_hash == 0) {
                int32_t count =
                    hk::func_call<int32_t>(0x1402A6CA0, &rc, 0xC160E555); // CGameObjectUtil::CountSubComponents

                std::vector<std::vector<std::pair<uint32_t, float>>> model_infos(count);
                uint8_t                                              slot = 0;

                CRuntimeContainer sub_rc{rc.m_base};
                for (auto a = rc.begin(); a != rc.end(); ++a) {
                    sub_rc.m_container = a;

                    if (sub_rc.GetHash(_class_hash) == 0xC160E555) { // some model container
                        CRuntimeContainer sub_sub_rc{sub_rc.m_base};
                        for (auto b = sub_rc.begin(); b != sub_rc.end(); ++b) {
                            sub_sub_rc.m_container = b;

                            if (sub_sub_rc.GetHash(_class_hash) == 0xAA09A7DC) { // model
                                model_hash   = sub_sub_rc.GetHash("filepath"_hash_little);
                                float weight = sub_sub_rc.GetFloat("Weight"_hash_little);

                                model_infos[slot].push_back({model_hash, weight});
                                break;
                            }
                        }

                        ++slot;
                    }
                }

                // @TODO: need to add tint stuff, so we don't have mismatched model parts!
                model_state->Reset();
                for (const auto& info : model_infos) {
                    if (info.empty()) {
                        continue;
                    }

                    model_state->SetModel(model_state->m_modelCount, info[0].first, character->m_worldTransform,
                                          &animated_model->m_animationController->m_skinningPalette);
                }

                // @CRASHFIX: make sure this doesn't crash the game.
                // hk::func_call<void>(0x1485D0300, ((char*)character + 0x10), 0.5f); // CCharacter::SetOpacity
            } else {
                model_state->Reset();
                model_state->SetModel(0, model_hash, character->m_worldTransform,
                                      &animated_model->m_animationController->m_skinningPalette);
            }
            break;
        }
    }

#ifdef DUMP_BONE_MAP_STUFF
    // DumpBoneMap(character);
    jc::CSpawnSystem::instance().Spawn("titan_enemy_001", character->m_worldTransform,
                                       [](const jc::spawned_objects& objects, void*) {
                                           // objects[2] isn't a guarentee to be a CSkinSwapper instance.
                                           // if a crash happens here, check the objects array for the correct index for
                                           // this entity!
                                           DumpBoneMap(objects[2].get());
                                       });
#endif

    SkeletonLookup::Get()->Empty();

    // rico skeleton		= 0xec0124bc (animations/skeletons/characters/rico.bsk)
    // other skeleton		= 0x7eecb71b (animations/skeletons/characters/world_sim.bsk)
    // combatant skeleton	= 0x5bdbe118 (animations/skeletons/characters/combatant.bsk)
    // titan skeleton		= 0x81425e27 (animations/skeletons/characters/titan.bsk)
    if (skeleton_hash == 0x7eecb71b || skeleton_hash == 0x5bdbe118 || skeleton_hash == 0x81425e27) {
        // fix bone mapping
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
    } else {
#ifdef _DEBUG
        __debugbreak();
#endif
    }
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
