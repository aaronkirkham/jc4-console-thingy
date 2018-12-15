#pragma once

#include "command.h"

class WorldCommand : public ICommand
{
  public:
    // clang-format off
    std::array<const char*, 2> m_Hints = {
        "time",
        "timescale",
    };
    // clang-format on

    virtual const char* GetCommand() override
    {
        return "world";
    }

    virtual bool Handler(const std::string& arguments) override
    {
        static auto WorldTime = *(void**)0x142A4B270;

        // time
        if (arguments.find("time ") != std::string::npos) {
            float time = 0.0f;
            if (sscanf_s(arguments.c_str(), "time %f", &time) == 1) {
                time = std::clamp(time, -24.0f, 24.0f);
                hk::func_call<void>(0x144A0D340, WorldTime, time, 2);
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

        return false;
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
