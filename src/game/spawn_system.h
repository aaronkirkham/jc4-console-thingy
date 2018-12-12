#pragma once

#include <string>

#include "hooking/hooking.h"

#pragma pack(push, 1)
namespace jc
{
using spawned_objects = std::vector<std::shared_ptr<CGameObject>>;

class CSpawnSystem
{
  public:
    static CSpawnSystem& instance()
    {
        return **(CSpawnSystem**)0x142A58BF0;
    }

    void Spawn(const std::string& model_name, const CMatrix4f& transform,
               std::function<void(const spawned_objects&, void*)> callback, void* userdata = nullptr)
    {
        using callback_t = void(__cdecl*)(const spawned_objects&, void*);

        struct SpawnReq {
            std::function<void(const spawned_objects&, void*)> callback;
            void*                                              userdata;
        };

        auto request = new SpawnReq{callback, userdata};
        hk::func_call<void>(
            0x140A3B430, this, model_name.c_str(), transform, 0x597Cu,
            (callback_t)[](const spawned_objects& objects, void* userdata) {
                auto spawn_req = (SpawnReq*)userdata;
                spawn_req->callback(objects, spawn_req->userdata);
                delete spawn_req;
            },
            request, nullptr, 0, -1);
    }
};
} // namespace jc
#pragma pack(pop)
