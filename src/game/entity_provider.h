#pragma once

#include <cstdint>

#include "allocator.h"
#include "shared_string.h"

namespace jc
{
#pragma pack(push, 1)
struct SRuntimeContainer {
    uint32_t m_key;
    uint32_t m_dataOffset;
    uint16_t m_numVariants;
    uint16_t m_numContainers;
};

class CRuntimeContainer
{
  public:
    uint64_t           m_base      = 0;
    SRuntimeContainer *m_container = nullptr;

  public:
    SRuntimeContainer *begin() const
    {
        if (!m_container || m_container->m_numContainers == 0) {
            return nullptr;
        }

        // see: 0x1478C93B0
        return (SRuntimeContainer *)((m_base + m_container->m_numVariants + 8 * m_container->m_numVariants
                                      + m_container->m_dataOffset + 3)
                                     & 0xFFFFFFFFFFFFFFFCu);
    }

    SRuntimeContainer *end() const
    {
        uint64_t begin = 0;
        if (m_container && m_container->m_numContainers > 0) {
            begin =
                ((m_base + m_container->m_numVariants + 8 * m_container->m_numVariants + m_container->m_dataOffset + 3)
                 & 0xFFFFFFFFFFFFFFFCu);
        }

        return (SRuntimeContainer *)(begin + 12 * (m_container ? m_container->m_numContainers : 0));
    }

    uint32_t GetHash(const uint32_t key)
    {
        uint32_t hash = 0;
        meow_hook::func_call<void>(GetAddress(RUNTIME_CONTAINER_GET_HASH), this, key, &hash);
        return hash;
    }
};

class CResourceLoader
{
  public:
    virtual ~CResourceLoader() = 0;

  public:
    char          _pad[0x38];
    CSharedString m_filename;
    char          _pad2[0xA150];
};

class CEntityResourceLoader : public CResourceLoader
{
  public:
    std::unique_ptr<CRuntimeContainer> m_propertyContainer;
};

class CEntityProvider
{
  public:
    std::unique_ptr<CEntityResourceLoader> m_entityResourceLoader;
    char                                   _pad[0x10];
    int32_t                                m_priority;
    int32_t                                m_streamerPriority;
    void *                                 m_resourceCache;
    bool                                   m_stop;
    bool                                   m_complete;
    char                                   _pad2[0x6];
};
static_assert(sizeof(CEntityProvider) == 0x30);
#pragma pack(pop)
}; // namespace jc
