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
        // ply.unlimitedammo.enable
        // ply.unlimitedammo.disable
        // ply.ammo.givemax

        hk::func_call<void>(0x147F69F60, arguments.c_str(), nullptr);
        return true;
    }

	virtual std::vector<std::string> GetHints(const std::string& arguments) override
    {
        return {};
    }
};
