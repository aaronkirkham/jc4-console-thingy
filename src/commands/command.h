#pragma once

#include <string>

using CommandHandler_t = bool(*)(const std::string&);

class ICommand
{
  public:
    virtual bool Handler(const std::string& arguments) = 0;
};
