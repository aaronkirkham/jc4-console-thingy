#pragma once

#include <Windows.h>
#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "commands/command.h"
#include "singleton.h"

class CommandHandler : public Singleton<CommandHandler>
{
  private:
    bool                     m_drawInput = false;
    std::vector<std::string> m_history;
    int32_t                  m_currentHistory;

    std::unordered_map<std::string, std::vector<CommandHandler_t>> m_commands;

  public:
    CommandHandler()          = default;
    virtual ~CommandHandler() = default;

    void Register(const std::string& command, CommandHandler_t fn)
    {
        m_commands[command].emplace_back(std::move(fn));
    }

    void EnableInput(bool toggle);
    bool IsInputEnabled() const
    {
        return m_drawInput;
    }

    void Draw();
    bool WndProc(uint32_t message, WPARAM wParam, LPARAM lParam);
};
