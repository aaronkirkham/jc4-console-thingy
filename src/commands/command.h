#pragma once

#include <string>

class ICommand
{
  public:
    virtual const char*						GetCommand() = 0;
    virtual void							Init(){};
    virtual std::pair<bool, std::string>	Handler(const std::string& arguments)  = 0; // Returns success, and command output (if any)
    virtual std::vector<std::string>		GetHints(const std::string& arguments) = 0;
};
