#pragma once

#include "command.h"

#include <fstream>

class TeleportCommand : public ICommand
{
  public:
    virtual const char* GetCommand() override
    {
        return "tp";
    }

    virtual bool Handler(const std::string& arguments) override
    {
        if (arguments.find("save ") != std::string::npos) {
            std::ofstream outfile("savedcoords.txt", std::ios::app);

            char name[128] = {0};
            if (sscanf_s(arguments.c_str(), "save %[^\n]s", &name, 128) == 1) {
                outfile << name << ": ";
            }

            auto& transform = jc::CPlayerManager::instance().GetLocalPlayerCharacter()->m_transform;
            outfile << transform.m[3].x << " " << transform.m[3].y << " " << transform.m[3].z;
            outfile << std::endl;

            return true;
        } else {
            CVector3f position{};
            if (sscanf_s(arguments.c_str(), "%f %f %f", &position.x, &position.y, &position.z) == 3) {
                CMatrix4f world{};
                world.m[3].x = position.x;
                world.m[3].y = position.y;
                world.m[3].z = position.z;

                jc::CGameWorld::instance().TeleportPlayer(&world, nullptr, false, false, 0.0f, 0.0f, 0, nullptr, false,
                                                          nullptr);
                return true;
            }
        }

        return false;
    }

    virtual std::vector<std::string> GetHints(const std::string& arguments) override
    {
        static std::array hints{
            "save",
        };

        std::vector<std::string> result;
        std::copy_if(hints.begin(), hints.end(), std::back_inserter(result),
                     [&](const std::string& item) { return item.find(arguments) != std::string::npos; });

        return result;
    }
};
