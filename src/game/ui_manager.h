#pragma once

#include <cstdint>

#pragma pack(push, 1)
namespace jc
{
class CUIBase
{
  public:
    char        _pad[0x140];
    uint8_t     m_state;
    char        _pad2[0x1F];
    std::string m_name;
    uint32_t    m_nameHash;
};

class CUIManager
{
  public:
    static CUIManager& instance()
    {
        return **(CUIManager**)0x142C27848;
    }

    CUIBase* GetUI(uint32_t name_hash)
    {
        auto it = std::find_if(m_uis.begin(), m_uis.end(),
                               [name_hash](const CUIBase* ui) { return ui->m_nameHash == name_hash; });
        return (it != m_uis.end()) ? (*it) : nullptr;
    }

  public:
    char                  _pad[0x380];
    std::vector<CUIBase*> m_uis;
};
}; // namespace jc
#pragma pack(pop)
