#pragma once

#include "command.h"

class EventCommand : ICommand
{
  public:
    static bool Handler(const std::string &arguments)
    {
        hk::func_call<void>(0x147F69F60, arguments.c_str(), nullptr);
        return true;
    }
};
