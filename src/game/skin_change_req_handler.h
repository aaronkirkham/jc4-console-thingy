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
};

class SkeletonLookup : public Singleton<SkeletonLookup>
{
  public:
    std::map<void *, std::map<CModelRenderBlock *, int16_t *>> m_lookup;
    std::mutex                                                 m_mutex;

  public:
    SkeletonLookup()          = default;
    virtual ~SkeletonLookup() = default;

    void Make(void *rbi_info, CModelRenderBlock *render_block);
    void Empty();
};
}; // namespace jc
