#include <Windows.h>

#include "graphics.h"
#include "input.h"
#include "vector.h"

#include "hooking/hooking.h"

#include "game/graphics_engine.h"
#include "game/render_engine.h"

void Input::EnableInput(bool toggle)
{
    static void *input_thingy = nullptr;
    if (!input_thingy) {
        input_thingy = *(void **)0x142A86B70;
        m_history.push_back("");
    }

    m_drawInput      = toggle;
    m_currentHistory = 0;

    if (toggle) {
        // resets keys so we don't have keys stuck after giving input back
        hk::func_call<void>(0x140E18EF0, input_thingy);
    } else {
        // restore
        hk::func_call<void>(0x140E18E40, input_thingy);
    }
}

void Input::Draw()
{
    auto device         = jc::NGraphicsEngine::CGraphicsEngine::instance().m_device;
    auto debug_renderer = jc::CRenderEngine::instance().m_debugRenderer;

    if (m_drawInput && (device && debug_renderer)) {
#ifdef DEBUG
        if (m_cmd) {
            const auto &hints = m_cmd->GetHints(m_cmdArguments);
            const auto  count = std::clamp<uint32_t>(hints.size(), 0, 10);

            float text_height          = (16.0f * count);
            float text_height_relative = (text_height / device->m_screenHeight);

            debug_renderer->DebugRectGradient({0, (0.93f - text_height_relative)},
                                              {0.5f, 0.93f}, 0xE1000000, 0x00000000);

            for (uint32_t i = 0; i < count; ++i) {
                //float text_y = (((0.92f * device->m_screenHeight) + (0.005f * device->m_screenHeight)) - (16.0f * i));
                //Graphics::Get()->DrawString(hints[i], {(0.0195f * device->m_screenWidth), text_y}, 12, 0xFFFFFFFF);
            }
        }
#endif

		// draw background box
        debug_renderer->DebugRectGradient({0, 0.95f}, {0.5f, 1}, 0xE1000000, 0x00000000);

        // render input text
        Graphics::Get()->DrawString("> ", {(0.0078f * device->m_screenWidth), (0.965f * device->m_screenHeight)}, 14,
                                    0xFFFFFFFF);
        Graphics::Get()->DrawString(
            m_history[0], {(0.0195f * device->m_screenWidth), (0.965f * device->m_screenHeight)}, 14, 0xFFFFFFFF);
    }
}

bool Input::WndProc(uint32_t message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_KEYDOWN: {
            // tilde key down and the previous key state was up
            const auto vsc = MapVirtualKeyEx(static_cast<int32_t>(wParam), MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
            if (vsc == 41 && (static_cast<uint32_t>(lParam) >> 30) == 0) {
                EnableInput(!IsInputEnabled());
                return true;
            }

            // handle history navigation
            if (m_drawInput) {
                switch (wParam) {
                    case VK_ESCAPE: {
                        EnableInput(false);
                        return true;
                    }

                    case VK_UP: {
                        if (m_currentHistory == 0) {
                            m_currentHistory = (m_history.size() - 1);
                        } else if (m_currentHistory > 1) {
                            --m_currentHistory;
                        }

                        m_history[0] = m_history[m_currentHistory];
                        UpdateCurrentCommand();

                        return true;
                    }

                    case VK_DOWN: {
                        if (m_currentHistory > 0 && m_currentHistory < (m_history.size() - 1)) {
                            ++m_currentHistory;
                            m_history[0] = m_history[m_currentHistory];
                        } else if (m_currentHistory == m_history.size()) {
                            m_currentHistory = 0;
                            m_history[0].clear();
                        }

                        UpdateCurrentCommand();

                        return true;
                    }
                }
            }

            break;
        }

        // handle input
        case WM_CHAR: {
            if (m_drawInput && wParam > 0 && wParam < 0x10000) {
                // ignore the tilde key, because we handle the toggle stuff in WM_KEYDOWN
                // we don't want the key of that event leaking into our input text
                if (((lParam >> 16) & 0xff) == 41) {
                    break;
                }

                switch (wParam) {
                    case VK_RETURN: {
                        const auto input_text = m_history[0];
                        m_history[0].clear();
                        m_currentHistory = 0;

                        if (m_cmd) {
                            if (m_cmd->Handler(m_cmdArguments)) {
                                AddToHistory(input_text);
                            }
                        } else if (m_fnCommands.size() > 0) {
                            const auto it = m_fnCommands.find(m_cmdText);
                            if (it != m_fnCommands.end()) {
                                (*it).second(m_cmdArguments);
                                AddToHistory(input_text);
                            }
                        }

                        m_cmd = nullptr;
                        m_cmdText.clear();
                        m_cmdArguments.clear();
                        EnableInput(false);
                        break;
                    }

                    case VK_TAB: {
                        break;
                    }

                    case VK_BACK: {
                        if (m_history[0].size() > 0) {
                            m_history[0].pop_back();
                            UpdateCurrentCommand();
                        }

                        break;
                    }

                    default: {
                        m_history[0].push_back((unsigned short)wParam);
                        UpdateCurrentCommand();
                        break;
                    }
                }
            }

            break;
        }
    }

    // if we are drawing the input, don't let the game have any inputs
    if (m_drawInput) {
        return true;
    }

    return false;
}

bool Input::UpdateCurrentCommand()
{
    const auto input_text = m_history[0];
    if (input_text.size() > 0) {
        const auto idx = input_text.find_first_of(' ');
        if (idx != std::string::npos) {
            m_cmdText      = input_text.substr(0, idx);
            m_cmdArguments = input_text.substr(idx + 1, input_text.length());
        } else {
            m_cmdText = input_text;
        }

        auto it = m_commands.find(m_cmdText);
        if (it != m_commands.end()) {
            m_cmd = (*it).second.get();
            return true;
        }
    }

    m_cmd = nullptr;
    return false;
}

void Input::AddToHistory(const std::string &command)
{
    if (m_history.back() != command) {
        m_history.push_back(command);

        // clear after 100 entries
        if (m_history.size() > 100) {
            m_history.clear();
            m_history[0] = "";
        }
    }
}
