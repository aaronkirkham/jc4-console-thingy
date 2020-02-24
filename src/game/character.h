#pragma once

#include "../vector.h"

#pragma pack(push, 1)
namespace jc
{
class CCharacter
{
  public:
    char      _pad[0x114];
    CMatrix4f m_transform;
    char      _pad2[0x2134];
    bool      m_unlimitedAmmo;
};
}; // namespace jc
#pragma pack(pop)
