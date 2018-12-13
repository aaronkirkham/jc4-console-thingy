#pragma once

#include "command.h"

class EventCommand : public ICommand
{
  public:
    // clang-format off
    std::array<const char*, 3> m_Hints = {
        "ply.unlimitedammo.enable",
        "ply.unlimitedammo.disable",
        "ply.ammo.givemax",
    };
    // clang-format on

    virtual const char* GetCommand() override
    {
        return "event";
    }

    virtual bool Handler(const std::string& arguments) override
    {
        // ply.unlimitedammo.enable
        // ply.unlimitedammo.disable
        // ply.ammo.givemax

        if (arguments == "load_game" || arguments == "new_game" || arguments == "continue_game") {
            return false;
        }

        hk::func_call<void>(0x147F69F60, arguments.c_str(), nullptr);
        return true;
    }

    virtual std::vector<std::string> GetHints(const std::string& arguments) override
    {
        return {};
    }
};
