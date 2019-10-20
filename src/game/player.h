#pragma once

#include "character.h"

#pragma pack(push, 1)
namespace jc
{
class CPlayerAimControl
{
  public:
    char      _pad[0x1E4];
    CVector3f m_aimPos;
};

class CPlayer
{
  public:
    char               _pad[0x68];
    CCharacter*        m_character;
    char               _pad2[0x150];
    CPlayerAimControl* m_aimControl;
};
}; // namespace jc
#pragma pack(pop)
