#pragma once

#include <string>

#include "../allocator.h"
#include "../vector.h"
#include "game_object.h"
#include "shared_string.h"

#include "hooking/hooking.h"

#pragma pack(push, 1)
namespace jc
{
using spawned_objects = std::vector<std::shared_ptr<CGameObject>>;

class CSpawnSystem
{
  public:
    struct SResourceDef {
        CSharedString m_resourcePath;
        char          _pad[0x178];
    };

    static CSpawnSystem& instance()
    {
        return **(CSpawnSystem**)0x142CB1D20;
    }

    void Spawn(const std::string& model_name, const CMatrix4f& transform,
               std::function<void(const spawned_objects&, void*)> callback, void* userdata = nullptr)
    {
        using success_t = void(__cdecl*)(const spawned_objects&, void*);
        using status_t  = void(__cdecl*)(int32_t, void*);

        struct SpawnReq {
            std::function<void(const spawned_objects&, void*)> callback;
            void*                                              userdata;
        };

        auto request = new SpawnReq{callback, userdata};
        hk::func_call<void>(
            0x140BADC60, this, model_name.c_str(), transform, 0x597Cu,
            (success_t)[](const spawned_objects& objects, void* userdata) {
                auto spawn_req = (SpawnReq*)userdata;
                spawn_req->callback(objects, spawn_req->userdata);
                delete spawn_req;
            },
            request,
            (status_t)[](int32_t status, void* userdata) {
                // resource failed to load
                if (status == 16) {
                    auto spawn_req = (SpawnReq*)userdata;
                    delete spawn_req;
                }
            },
            0, -1);
    }

    bool GetMatchingResources(const std::string&                                        model_name,
                              std::vector<SResourceDef*, jc::allocator<SResourceDef*>>* out_resources)
    {
        int16_t tags[32]     = {};
        int32_t invalid_tags = 0;

        // CSpawnSystem::ParseTags
        auto count = hk::func_call<int64_t>(0x140B9CA80, this, model_name.c_str(), &tags, &invalid_tags);
        if (!count) {
            // look in the resource defs (to support absolute model paths)
            for (decltype(m_resourceDefCount) i = 0; i < m_resourceDefCount; ++i) {
                if (m_resourceDefs[i].m_resourcePath.m_str->compare(model_name.c_str()) == 0) {
                    out_resources->push_back(&m_resourceDefs[i]);
                    return true;
                }
            }

            return false;
        }

        // CSpawnSystem::GetMatchingResources
        hk::func_call<void>(0x140B88770, this, &tags, out_resources, false, "", 0xDEADBEEF);
        return !out_resources->empty();
    }

  public:
    char                                          _pad[0xB4];
    uint32_t                                      m_resourceDefCount;
    char                                          _pad2[0x8];
    std::unique_ptr<CSpawnSystem::SResourceDef[]> m_resourceDefs;
    char                                          _pad3[0x68];
    void*                                         m_resourceCache;
};
} // namespace jc
#pragma pack(pop)
