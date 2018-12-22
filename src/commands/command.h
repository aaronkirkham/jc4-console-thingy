#pragma once

#include <string>

class ICommand
{
  public:
    virtual void Initialize()
    {
        return;
    }

    virtual const char*              GetCommand()                           = 0;
    virtual bool                     Handler(const std::string& arguments)  = 0;
    virtual std::vector<std::string> GetHints(const std::string& arguments) = 0;
};
