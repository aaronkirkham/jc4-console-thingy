#pragma once

#include "command.h"

class EventCommand : public ICommand
{
  public:
    // clang-format off
    std::array<const char*, 10> m_Hints = {
        "__showfps",
        "__reload_world",
        "ply.unlimitedammo.enable",
        "ply.unlimitedammo.disable",
        "ply.ammo.givemax",
        "ply.pause",
        "ply.unpause",
        "ply.vehicle.burn",
        "moon_gravity_on",
        "moon_gravity_off",
    };
    // clang-format on

    virtual const char* GetCommand() override
    {
        return "event";
    }

    virtual bool Handler(const std::string& arguments) override
    {
        if (arguments == "load_game" || arguments == "new_game" || arguments == "continue_game") {
            return false;
        }

        hk::func_call<void>(0x1447428C0, arguments.c_str(), nullptr);
        return true;
    }

    virtual std::vector<std::string> GetHints(const std::string& arguments) override
    {
        std::vector<std::string> result;
        std::copy_if(m_Hints.begin(), m_Hints.end(), std::back_inserter(result), [&](const std::string& item) {
            return (result.size() < 10) ? (item.find(arguments) != std::string::npos) : false;
        });

        return result;
    }
};
