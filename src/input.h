#pragma once

#include <Windows.h>
#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "commands/command.h"
#include "singleton.h"

class Input : public Singleton<Input>
{
    using command_t = std::function<void(const std::string& arguments)>;

  private:
    bool                     m_drawInput = false;
    std::vector<std::string> m_history;
    int32_t                  m_currentHistory;
    ICommand*                m_cmd          = nullptr;
    std::string              m_cmdText      = "";
    std::string              m_cmdArguments = "";

    std::unordered_map<std::string, std::unique_ptr<ICommand>> m_commands;
    std::unordered_map<std::string, command_t>                 m_fnCommands;

    bool UpdateCurrentCommand();
    void AddToHistory(const std::string& command);

  public:
    Input()          = default;
    virtual ~Input() = default;

    void RegisterCommand(std::unique_ptr<ICommand> cmd)
    {
        m_commands[cmd->GetCommand()] = std::move(cmd);
    }

    void RegisterCommand(const std::string& command, std::unique_ptr<ICommand> cmd)
    {
        m_commands[command] = std::move(cmd);
    }

    void RegisterCommand(const std::string& command, command_t fn)
    {
        m_fnCommands[command] = std::move(fn);
    }

    void EnableInput(bool toggle);
    bool IsInputEnabled() const
    {
        return m_drawInput;
    }

    void Draw();
    bool WndProc(uint32_t message, WPARAM wParam, LPARAM lParam);
};
