#include "graphics.h"

#include "input.h"
#include "util.h"

#include "game/device.h"

int64_t Graphics::GraphicsFlipCallback(jc::HDevice_t* device)
{
    const auto& gfx = Graphics::Get();

    if (!gfx->m_ready && device) {
        IFW1Factory* factory = nullptr;
        auto         hr      = FW1CreateFactory(FW1_VERSION, &factory);

        if (SUCCEEDED(hr)) {
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

            hr = factory->CreateFontWrapper(device->m_device, nullptr, &createParams, &gfx->m_font);
            if (SUCCEEDED(hr)) {
                gfx->m_ready  = true;
                gfx->m_device = device;
            }

            factory->Release();
        }
    }

    // save the current state
    if (device) {
        gfx->m_restoreState = SUCCEEDED(gfx->m_state.saveCurrentState(device->m_deviceContext));
    }

    Input::Get()->Draw();

    if (gfx->m_restoreState) {
        gfx->m_state.restoreSavedState();
    }

    return pfn_Flip(device);
}

void Graphics::Shutdown()
{
    m_ready = false;
    m_state.releaseSavedState();

    if (m_font) {
        m_font->Release();
    }
}

void Graphics::DrawString(const std::string& str, float x, float y, float size, uint32_t color)
{
    if (!m_ready || !m_device) {
        return;
    }

    x    = (x * m_device->m_screenWidth);
    y    = (y * m_device->m_screenHeight);
    size = (size * m_device->m_screenHeight);

    m_font->DrawString(m_device->m_deviceContext, util::ToWideString(str).c_str(), size, x, y, color, 0);
}
