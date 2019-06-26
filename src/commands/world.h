#pragma once

#include "command.h"

class WorldCommand : public ICommand
{
  public:
    std::array<const char*, 4> m_Hints = {
        "time",
        "timescale",
        "gravity",
        "resetgravity",
    };

    virtual const char* GetCommand() override
    {
        return "world";
    }

    virtual bool Handler(const std::string& arguments) override
    {
        static auto World     = *(void**)0x142C7FEF0;
        static auto WorldTime = *(void**)0x142C4C1A0;

        // time
        if (arguments.find("time ") != std::string::npos) {
            float time = 0.0f;
            if (sscanf_s(arguments.c_str(), "time %f", &time) == 1) {
                time = std::clamp(time, -24.0f, 24.0f);
                hk::func_call<void>(0x14031C800, WorldTime, time, 2);
                return true;
            }
        }
        // time scale
        else if (arguments.find("timescale ") != std::string::npos) {
            float timescale = 1.0f;
            if (sscanf_s(arguments.c_str(), "timescale %f", &timescale) == 1) {
                *(float*)((char*)WorldTime + 0xE4) = std::clamp(timescale, -1000.0f, 1000.0f);
                return true;
            }
        }
        // gravity
        else if (arguments.find("gravity ") != std::string::npos) {
            float gravity = -9.810f;
            if (sscanf_s(arguments.c_str(), "gravity %f", &gravity) == 1) {
                *(float*)((char*)World + 0x974) = std::clamp(gravity, -5000.0f, 5000.0f);
                return true;
            }
        }
        // reset gravity
        else if (arguments.find("resetgravity") != std::string::npos) {
            *(float*)((char*)World + 0x974) = -9.810f;
            return true;
        }

        return false;
    }

    virtual std::vector<std::string> GetHints(const std::string& arguments) override
    {
        std::vector<std::string> result;
        std::copy_if(m_Hints.begin(), m_Hints.end(), std::back_inserter(result),
                     [&](const std::string& item) { return item.find(arguments) != std::string::npos; });

        return result;
    }
};
