#pragma once

#include <FW1FontWrapper.h>
#include <codecvt>

#include "game/device.h"
#include "singleton.h"
#include "vector.h"

class Graphics : public Singleton<Graphics>
{
  private:
    bool             m_ready  = false;
    jc::HDevice_t*   m_device = nullptr;
    IFW1FontWrapper* m_font   = nullptr;

  public:
    Graphics()  = default;
    ~Graphics() = default;

    void Init(jc::HDevice_t* device)
    {
        if (!m_ready && device) {
            IFW1Factory* factory = nullptr;
            auto         hr      = FW1CreateFactory(FW1_VERSION, &factory);

            if (SUCCEEDED(hr) && factory) {
                FW1_FONTWRAPPERCREATEPARAMS createParams{};
                createParams.GlyphSheetWidth                 = 512;
                createParams.GlyphSheetHeight                = 512;
                createParams.MaxGlyphCountPerSheet           = 2048;
                createParams.SheetMipLevels                  = 1;
                createParams.AnisotropicFiltering            = FALSE;
                createParams.MaxGlyphWidth                   = 384;
                createParams.MaxGlyphHeight                  = 384;
                createParams.DisableGeometryShader           = FALSE;
                createParams.VertexBufferSize                = 0;
                createParams.DefaultFontParams.pszFontFamily = L"Arial";
                createParams.DefaultFontParams.FontWeight    = DWRITE_FONT_WEIGHT_SEMI_BOLD;
                createParams.DefaultFontParams.FontStyle     = DWRITE_FONT_STYLE_NORMAL;
                createParams.DefaultFontParams.FontStretch   = DWRITE_FONT_STRETCH_NORMAL;
                createParams.DefaultFontParams.pszLocale     = L"";

                hr = factory->CreateFontWrapper(device->m_device, nullptr, &createParams, &m_font);

                if (SUCCEEDED(hr)) {
                    m_ready  = true;
                    m_device = device;
                }

                factory->Release();
            }
        }
    }

    void DrawString(const std::string& str, const CVector2f& position, float size, uint32_t color)
    {
        if (!m_ready) {
            return;
        }

        std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> converter;
        m_font->DrawString(m_device->m_deviceContext, converter.from_bytes(str).c_str(), size, position.x, position.y,
                           color, FW1_RESTORESTATE);
    }
};
