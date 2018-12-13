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
    m_currentHint    = -1;

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
        // draw hints
        if (m_cmd && m_hints.size() > 0) {
            const auto count = std::clamp<uint32_t>(m_hints.size(), 0, 10);

            static constexpr float item_height = 18.0f;

            // draw background box
            float text_height = ((item_height * count) / device->m_screenHeight);
            debug_renderer->DebugRectGradient({0, (0.94f - text_height)}, {0.5f, 0.94f}, 0xB4000000, 0x00000000);

            // draw hints
            for (uint32_t i = 0; i < count; ++i) {
                float text_y = ((0.94f * device->m_screenHeight) - (item_height * count)) + (item_height * i);

                if (m_currentHint == i) {
                    Graphics::Get()->DrawString("> ", {(0.0078f * device->m_screenWidth), text_y}, 12, 0xFFFFFFFF);
                }

                Graphics::Get()->DrawString(m_hints[i], {(0.0195f * device->m_screenWidth), text_y}, 12, 0xFFFFFFFF);
            }
        }

        // draw background box
        debug_renderer->DebugRectGradient({0, 0.95f}, {0.5f, 1}, 0xE1000000, 0x00000000);

        if (m_currentHint == -1) {
            Graphics::Get()->DrawString("> ", {(0.0078f * device->m_screenWidth), (0.965f * device->m_screenHeight)},
                                        14, 0xFFFFFFFF);
        }

        // render input text
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
                        if (m_currentHint == -1) {
                            if (m_history[0].size() > 0) {
                                m_history[0].clear();
                                m_cmdText        = "";
                                m_cmdArguments   = "";
                                m_currentHistory = 0;
                                m_hints.clear();
                            } else {
                                EnableInput(false);
                            }
                        } else {
                            m_currentHint = -1;
                        }

                        return true;
                    }

                    case VK_UP: {
                        // previous history item
                        if (m_currentHint == -1) {
                            if (m_currentHistory == 0) {
                                m_currentHistory = (m_history.size() - 1);
                            } else if (m_currentHistory > 1) {
                                --m_currentHistory;
                            }

                            m_history[0] = m_history[m_currentHistory];
                            UpdateCurrentCommand(false);
                            m_hints.clear();
                        }
                        // previous hint item
                        else {
                            auto size     = m_hints.size();
                            m_currentHint = std::clamp<int32_t>(--m_currentHint, 0, size < 10 ? size : 10);
                        }

                        return true;
                    }

                    case VK_DOWN: {
                        // next history item
                        if (m_currentHint == -1) {
                            if (m_currentHistory > 0 && m_currentHistory < (m_history.size() - 1)) {
                                ++m_currentHistory;
                                m_history[0] = m_history[m_currentHistory];
                            } else if (m_currentHistory == (m_history.size() - 1)) {
                                m_currentHistory = 0;
                                m_history[0].clear();
                            }

                            UpdateCurrentCommand(false);
                            m_hints.clear();
                        }
                        // next hint item
                        else {
                            auto size     = m_hints.size();
                            m_currentHint = std::clamp<int32_t>(++m_currentHint, 0, size < 10 ? size : 10);
                        }

                        return true;
                    }

                    // move focus into the hints list
                    case VK_TAB: {
                        if (m_currentHint == -1 && m_hints.size() > 0) {
                            m_currentHint = 0;
                        }

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
                        // use the current hint as the command argument
                        if (m_currentHint != -1) {
                            m_cmdArguments = m_hints[m_currentHint];
                            m_history[0]   = (m_cmdText + " " + m_cmdArguments);
                            m_currentHint  = -1;
                            m_hints.clear();
                            break;
                        }

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

                    case VK_BACK: {
                        m_currentHint = -1;

                        if (m_history[0].size() > 0) {
                            m_history[0].pop_back();
                            UpdateCurrentCommand();
                        }

                        break;
                    }

                    case VK_ESCAPE:
                    case VK_TAB: {
                        break;
                    }

                    default: {
                        m_currentHint = -1;
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

bool Input::UpdateCurrentCommand(bool update_hints)
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
            if (update_hints) {
                m_hints = m_cmd->GetHints(m_cmdArguments);
            }

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
