#pragma once

#include "singleton.h"

#include "shared_string.h"

#include <functional>
#include <map>
#include <mutex>
#include <string>

namespace jc
{
struct CModelRenderBlock;
class CRuntimeContainer;
class CEntityProvider;
class SkinChangeRequestHandler : public Singleton<SkinChangeRequestHandler>
{
    using entity_provider_callback_t = void(__cdecl *)(void *);

  public:
    CEntityProvider *                              m_provider = nullptr;
    std::function<void(const CRuntimeContainer *)> m_callback = nullptr;

  public:
    SkinChangeRequestHandler();
    void Request(CSharedString &resource_path, std::function<void(const CRuntimeContainer *)>);
    void Update();

    static void DrawSkinBatches(CModelRenderBlock *render_block, void *render_context, void *rbi_info, bool unknown);
};

class SkeletonLookup : public Singleton<SkeletonLookup>
{
  public:
    std::vector<void *>                      m_rbiInstances;
    std::map<CModelRenderBlock *, int16_t *> m_skeletonLookup;
    std::mutex                               m_mutex;

  public:
    SkeletonLookup()          = default;
    virtual ~SkeletonLookup() = default;

    void SetRBIInfo(void *rbi_info)
    {
        std::lock_guard<std::mutex> lk{m_mutex};
        m_rbiInstances.push_back(rbi_info);
    }

    void Make(CModelRenderBlock *render_block);
    void Empty();
};
}; // namespace jc
