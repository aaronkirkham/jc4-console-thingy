#pragma once

#include "singleton.h"

#include <d3d11.h>

#include <CFW1StateSaver.h>
#include <FW1FontWrapper.h>

#include <string>

namespace jc
{
class HDevice_t;
};

class Graphics : public Singleton<Graphics>
{
  public:
    bool                           m_ready  = false;
    jc::HDevice_t*                 m_device = nullptr;
    IFW1FontWrapper*               m_font   = nullptr;
    FW1FontWrapper::CFW1StateSaver m_state{};
    bool                           m_restoreState = false;

  public:
    static int64_t                                GraphicsFlipCallback(jc::HDevice_t* device);
    static inline decltype(GraphicsFlipCallback)* pfn_Flip = nullptr;

  public:
    Graphics()  = default;
    ~Graphics() = default;

    void Shutdown();

    void DrawString(const std::string& str, float x, float y, float size, uint32_t color);
};
