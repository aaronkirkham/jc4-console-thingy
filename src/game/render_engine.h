#pragma once

#include "debug_renderer_impl.h"

#pragma pack(push, 1)
namespace jc
{
class CRenderEngine
{
  public:
    static CRenderEngine& instance()
    {
        return **(CRenderEngine**)GetAddress(INST_RENDER_ENGINE);
    }

  public:
    char               _pad[0x2BC8];
    DebugRendererImpl* m_debugRenderer;
};
}; // namespace jc
#pragma pack(pop)
