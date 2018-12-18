#pragma once

#include <cstdint>

#pragma pack(push, 1)
namespace jc
{
class CUIBase
{
  public:
    char    pad[0x140];
    uint8_t m_state;
};

class CUIManager
{
  public:
    static CUIManager& instance()
    {
        return **(CUIManager**)0x142A5EBF8;
    }

  public:
    char _pad[0x20A];
    bool m_enabled;
    char _pad2[0xCD];
    struct {
        char     _pad[0x38];
        CUIBase* m_vehicle;
        char     _pad2[0x8];
        CUIBase* m_bottomLeft;
    } * m_unknown;
};
}; // namespace jc
#pragma pack(pop)
