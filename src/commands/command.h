#pragma once

#include <string>

using CommandHandler_t = bool (*)(const std::string&);

class ICommand
{
  public:
    virtual const char*              GetCommand()                           = 0;
    virtual bool                     Handler(const std::string& arguments)  = 0;
    virtual std::vector<std::string> GetHints(const std::string& arguments) = 0;
};
