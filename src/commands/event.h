#pragma once

#include "command.h"

class EventCommand : public ICommand
{
  public:
    virtual const char* GetCommand() override
    {
        return "event";
    }

    virtual bool Handler(const std::string& arguments) override
    {
        using namespace jc;

        if (arguments == "load_game" || arguments == "new_game" || arguments == "continue_game") {
            return false;
        }

        meow_hook::func_call<void>(GetAddress(SEND_EVENT), arguments.c_str(), nullptr);
        return true;
    }

    virtual std::vector<std::string> GetHints(const std::string& arguments) override
    {
        static std::array hints{
            "__reload_world",
            "ply.invulnerable",
            "ply.vulnerable",
            "ply.unlimitedammo.enable",
            "ply.unlimitedammo.disable",
            "ply.ammo.givemax",
            "ply.pause",
            "ply.unpause",
            "ply.vehicle.burn",
            "moon_gravity_on",
            "moon_gravity_off",
            "debug.vehicle.incrementtint",
            "vocals.rico.enable",
            "vocals.rico.disable",
        };

        std::vector<std::string> result;
        std::copy_if(hints.begin(), hints.end(), std::back_inserter(result),
                     [&](const std::string& item) { return item.find(arguments) != std::string::npos; });

        return result;
    }
};
