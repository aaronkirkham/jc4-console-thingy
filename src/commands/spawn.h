#pragma once

#include "command.h"
#include <algorithm>
#include <iostream>
#include <set>

class SpawnCommand : public ICommand
{
    struct PropertyFileEntry {
        uint32_t hash;
        uint32_t data;
        uint8_t  type;
    };

    struct PropertyFileResult {
        uint8_t *          base;
        PropertyFileEntry *entry;
        uint8_t            error;
    };

    struct Spawnable {
        std::string              model;
        std::vector<std::string> specifiers;
    };

    inline static const std::string delims = ", |";

  public:
    inline static std::vector<Spawnable> m_Hints = {};

    virtual void Initialize() override
    {
        hk::inject_call<PropertyFileResult *, void *, PropertyFileResult *, uint32_t> add_event_hook(0x140A08310);
        add_event_hook.inject(AddModelName);
    }

    static std::vector<std::string> SplitSpecifiers(const std::string &input)
    {
        std::vector<std::string> result;
        size_t                   lastDelim = 0;
        size_t                   i;
        for (i = 0; i < input.length(); i++) {
            if (delims.find(input.at(i)) != std::string::npos) {
                if (i > lastDelim) {
                    result.push_back(std::string(&input[lastDelim], i - lastDelim));
                }
                lastDelim = i + 1;
            }
        }
        if (i > lastDelim) {
            result.push_back(std::string(&input[lastDelim], i - lastDelim));
        }
        return result;
    }

    static PropertyFileResult *AddModelName(void *file, PropertyFileResult *buf, uint32_t hash)
    {
        Spawnable          s;
        PropertyFileResult model;
        PropertyFileResult specifiers;

        hk::func_call<PropertyFileResult *>(0x140087E60, file, &model, 0xF71C2A21);
        hk::func_call<PropertyFileResult *>(0x140087E60, file, &specifiers, 0x54697E8F);

        if (model.entry && specifiers.entry) {
            s.model      = (char *)(model.base + model.entry->data);
            s.specifiers = SplitSpecifiers((char *)(specifiers.base + specifiers.entry->data));
            m_Hints.push_back(s);
        }

        return hk::func_call<PropertyFileResult *>(0x140087E60, file, buf, hash);
    }

    virtual const char *GetCommand() override
    {
        return "spawn";
    }

    virtual bool Handler(const std::string &arguments) override
    {
        std::string spawner;
        auto        local_player = jc::CPlayerManager::instance().m_localPlayer;
        auto        transform    = local_player->m_character->m_transform;
        auto &      aimpos       = local_player->m_aimControl->m_aimPos;

        transform.m[3].x = aimpos.x;
        transform.m[3].y = aimpos.y + 1.0f;
        transform.m[3].z = aimpos.z;

        if (arguments.length() == 0) {
            return false;
        }

        if (arguments.at(0) == '!') {
            spawner = arguments.substr(1);
        } else {
            spawner = arguments;
        }

        // TODO(aaronlad): figure out the flags, right now the game is holding the shared_ptr so we don't have to do it
        jc::CSpawnSystem::instance().Spawn(spawner, transform, [](const jc::spawned_objects &objects, void *) {
            //
        });

        return true;
    }

    virtual std::vector<std::string> GetHints(const std::string &arguments) override
    {
        if (arguments.length() > 0 && arguments.at(0) == '!') {
            std::string            content = arguments.substr(1);
            std::vector<Spawnable> intermediates;
            std::set<std::string>  results;

            std::vector<std::string> userSpecifiers = SplitSpecifiers(content);
            std::string              last           = "";
            if (userSpecifiers.size() > 0 && delims.find(content.back()) == std::string::npos) {
                last = userSpecifiers.back();
                userSpecifiers.pop_back();
            }

            std::string base = content.substr(0, content.length() - last.length());

            std::copy_if(m_Hints.begin(), m_Hints.end(), std::back_inserter(intermediates), [&](const Spawnable &item) {
                return std::all_of(userSpecifiers.begin(), userSpecifiers.end(), [&](const std::string &user) {
                    return std::find(item.specifiers.begin(), item.specifiers.end(), user) != item.specifiers.end();
                });
            });

            for (Spawnable &item : intermediates) {
                for (std::string &specifier : item.specifiers) {
                    if (specifier.find(last) != std::string::npos) {
                        results.insert("!" + base + specifier);
                    }
                }
            }

            return std::vector(results.begin(), results.end());
        } else {
            std::vector<Spawnable>   intermediates;
            std::vector<std::string> result;

            std::copy_if(m_Hints.begin(), m_Hints.end(), std::back_inserter(intermediates), [&](const Spawnable &item) {
                return item.model.find(arguments) != std::string::npos;
            });
            std::transform(intermediates.begin(), intermediates.end(), std::back_inserter(result),
                           [](const Spawnable &item) -> std::string { return item.model; });

            return result;
        }
    }
};
