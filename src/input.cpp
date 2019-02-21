#include <Windows.h>

#include "graphics.h"
#include "input.h"
#include "util.h"
#include "vector.h"

#include "hooking/hooking.h"

#include "game/graphics_engine.h"
#include "game/render_engine.h"
#include "game/ui_manager.h"

static const int32_t _numHintsPerPage = 10;

void Input::EnableInput(bool toggle)
{
    static void *input_thingy = nullptr;
    if (!input_thingy) {
        input_thingy = *(void **)0x142A8E6F0;
        m_history.push_back("");
    }

    m_drawInput      = toggle;
    m_currentHistory = 0;
    m_selectedHint   = -1;
    m_hintPage       = 0;

    // toggle hud which might get in the way (bottomleft)
    auto &ui_manager      = jc::CUIManager::instance();
    auto  hud_vehicle     = ui_manager.GetUI(0x8AC05ABA);
    auto  hud_bottom_left = ui_manager.GetUI(0x5F62FEDF);

    if (hud_vehicle && hud_bottom_left) {
        hud_vehicle->m_state     = toggle ? 1 : 2;
        hud_bottom_left->m_state = toggle ? 1 : 2;
    }

    if (toggle) {
        // resets keys so we don't have keys stuck after giving input back
        hk::func_call<void>(0x140E1DDB0, input_thingy);
    } else {
        // restore
        hk::func_call<void>(0x140E1DD00, input_thingy);
    }
}

void Input::Draw()
{
    auto device         = jc::NGraphicsEngine::CGraphicsEngine::instance().m_device;
    auto debug_renderer = jc::CRenderEngine::instance().m_debugRenderer;

    static const float _hintItemHeight = 0.02f;
    static const float _fontSizeInput  = 0.0155f;
    static const float _fontSizeHint   = 0.0133f;

    if (m_drawInput && (device && debug_renderer)) {
        // draw hints
        if (m_cmd && m_hints.size() > 0) {
            const auto  count        = std::clamp<uint32_t>((m_hints.size() - m_hintPage), 0, _numHintsPerPage);
            const float total_height = (_hintItemHeight * count);

            debug_renderer->DebugRectGradient({0, (0.94f - total_height - 0.02f)}, {0.5f, 0.94f}, 0xB4000000,
                                              0x00000000);

            for (uint32_t i = 0; i < count; ++i) {
                // draw current hint
                const float y = (0.9325f - total_height + (_hintItemHeight * i));
                Graphics::Get()->DrawString(m_hints[i + m_hintPage], 0.0195f, y, _fontSizeHint, 0xFFFFFFFF);

                if ((i + m_hintPage) == m_selectedHint) {
                    Graphics::Get()->DrawString("> ", 0.0078f, y, _fontSizeHint, 0xFFFFFFFF);
                }
            }
        }

        // draw current input text
        debug_renderer->DebugRectGradient({0, 0.95f}, {0.5f, 1}, 0xE1000000, 0x00000000);
        Graphics::Get()->DrawString(m_history[0], 0.0195f, 0.965f, _fontSizeInput, 0xFFFFFFFF);

        if (m_selectedHint == -1) {
            Graphics::Get()->DrawString("> ", 0.0078f, 0.965f, _fontSizeInput, 0xFFFFFFFF);
        }
    }
}

bool Input::WndProc(uint32_t message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_KEYDOWN: {
            // tilde key down and the previous key state was up
            const auto vsc = MapVirtualKeyEx(static_cast<int32_t>(wParam), MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
            if ((vsc == 41 || wParam == VK_F1) && (lParam >> 30) == 0) {
                EnableInput(!IsInputEnabled());
                return true;
            }

            // TODO(aaronlad): clean some of this up..

            // handle history navigation
            if (m_drawInput) {
                switch (wParam) {
                    // handle CTRL + V
                    case 0x56: {
                        if (GetAsyncKeyState(VK_CONTROL) && (lParam >> 30) == 0) {
                            m_selectedHint = -1;
                            m_hintPage     = 0;
                            m_history[0].append(util::GetClipboard());
                            UpdateCurrentCommand();
                            return true;
                        }

                        break;
                    }

                    case VK_ESCAPE: {
                        if (m_selectedHint == -1) {
                            if (m_history[0].size() > 0) {
                                m_history[0]     = "";
                                m_cmdText        = "";
                                m_cmdArguments   = "";
                                m_currentHistory = 0;
                                m_hints.clear();
                            } else {
                                EnableInput(false);
                            }
                        } else {
                            m_selectedHint = -1;
                            m_hintPage     = 0;
                        }

                        return true;
                    }

                    case VK_UP: {
                        // previous history item
                        if (m_selectedHint == -1) {
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
                            if ((m_selectedHint % _numHintsPerPage) == 0) {
                                m_hintPage = std::max((m_hintPage - _numHintsPerPage), 0);
                            }

                            m_selectedHint = std::max(--m_selectedHint, 0);
                        }

                        return true;
                    }

                    case VK_DOWN: {
                        // next history item
                        if (m_selectedHint == -1) {
                            if (m_currentHistory > 0 && m_currentHistory < (m_history.size() - 1)) {
                                ++m_currentHistory;
                                m_history[0] = m_history[m_currentHistory];
                            } else if (m_currentHistory == (m_history.size() - 1)) {
                                m_currentHistory = 0;
                                m_history[0]     = "";
                            }

                            UpdateCurrentCommand(false);
                            m_hints.clear();
                        }
                        // next hint item
                        else {
                            const auto size = m_hints.size();
                            if (m_selectedHint != (size - 1)) {
                                ++m_selectedHint;

                                if (size > _numHintsPerPage && (m_selectedHint % _numHintsPerPage) == 0) {
                                    m_hintPage += _numHintsPerPage;
                                }
                            }
                        }

                        return true;
                    }

                    // move focus into the hints list
                    case VK_TAB: {
                        if (m_selectedHint == -1 && m_hints.size() > 0) {
                            m_selectedHint = 0;
                            m_hintPage     = 0;
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

                // ignore input if the control key is down
                if (GetAsyncKeyState(VK_CONTROL)) {
                    break;
                }

                switch (wParam) {
                    case VK_RETURN: {
                        // use the current hint as the command argument
                        if (m_selectedHint != -1) {
                            m_cmdArguments = m_hints[m_selectedHint];
                            m_history[0]   = (m_cmdText + " " + m_cmdArguments);
                            m_selectedHint = -1;
                            m_hintPage     = 0;
                            m_hints.clear();
                            break;
                        }

                        const auto input_text = m_history[0];
                        m_history[0]          = "";
                        m_currentHistory      = 0;

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
                        m_selectedHint = -1;
                        m_hintPage     = 0;

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
                        m_selectedHint = -1;
                        m_hintPage     = 0;
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
            m_cmdText      = input_text;
            m_cmdArguments = "";
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
        // clear after 100 entries
        if (m_history.size() > 100) {
            m_history.clear();
            m_history.push_back("");
        }

        m_history.push_back(command);
    }
}
