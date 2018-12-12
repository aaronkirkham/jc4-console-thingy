#include <Windows.h>

#include "command_handler.h"

#include "../graphics.h"
#include "../vector.h"

#include "hooking/hooking.h"

void CommandHandler::EnableInput(bool toggle)
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

void CommandHandler::Draw()
{
    if (m_drawInput) {
        const std::string text = "> " + m_history[0];

        // top/left & top/right shadow
        Graphics::Get()->DrawString(text, {9, 149}, 14, 0x80000000);
        Graphics::Get()->DrawString(text, {9, 151}, 14, 0x80000000);

        // bottom/left & bottom/right shadow
        Graphics::Get()->DrawString(text, {11, 149}, 14, 0x80000000);
        Graphics::Get()->DrawString(text, {11, 151}, 14, 0x80000000);

        Graphics::Get()->DrawString(text, {10, 150}, 14, 0xFFFFFFFF);
    }
}

bool CommandHandler::WndProc(uint32_t message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case WM_KEYUP: {
            // tilde key
            const auto vsc = MapVirtualKeyEx(static_cast<int32_t>(wParam), MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
            if (vsc == 41) {
                EnableInput(!IsInputEnabled());
                return true;
            }

			// handle history navigation
            if (m_drawInput) {
                switch (wParam) {
                    case VK_UP: {
                        if (m_currentHistory == 0) {
                            m_currentHistory = (m_history.size() - 1);
                        } else if (m_currentHistory > 1) {
                            --m_currentHistory;
                        }

                        m_history[0] = m_history[m_currentHistory];

                        return true;
                    }

                    case VK_DOWN: {
                        if (m_currentHistory > 0 && m_currentHistory < (m_history.size() - 1)) {
                            ++m_currentHistory;
                            m_history[0] = m_history[m_currentHistory];
                        } else if (m_currentHistory == (m_history.size() - 1)) {
                            m_currentHistory = 0;
                            m_history[0].clear();
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
                switch (wParam) {
                    case VK_RETURN: {
                        const auto input_text = m_history[0];
                        if (input_text.size() > 0) {
                            std::string command   = "";
                            std::string arguments = "";

                            auto idx = input_text.find_first_of(' ');
                            if (idx != std::string::npos) {
                                command   = input_text.substr(0, idx);
                                arguments = input_text.substr(idx + 1, input_text.length());
                            } else {
                                command = input_text;
                            }

                            m_history[0].clear();
                            m_currentHistory = 0;

                            // handle the command
                            auto it = m_commands.find(command);
                            if (it != m_commands.end()) {
                                for (const auto &callback : (*it).second) {
                                    if (callback(arguments)) {
                                        // push history if it wasn't the same as before
                                        if (m_history.back() != input_text) {
                                            m_history.push_back(input_text);

                                            // clear after 100 entries
                                            if (m_history.size() > 100) {
                                                m_history.clear();
                                                m_history[0] = "";
                                            }
                                        }

                                        break;
                                    }
                                }
                            }
                        }

                        EnableInput(false);
                        break;
                    }

                    case VK_BACK: {
                        if (m_history[0].size() > 0) {
                            m_history[0].pop_back();
                        }

                        break;
                    }

                    default: {
                        m_history[0].push_back((unsigned short)wParam);
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
