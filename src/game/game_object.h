#pragma once

#include <cstdint>

#pragma pack(push, 1)
namespace jc
{
class CGameObject
{
  public:
    virtual ~CGameObject()           = 0;
    virtual void      function_1()   = 0;
    virtual void      function_2()   = 0;
    virtual uint32_t* GetClassHash() = 0;
};
} // namespace jc
#pragma pack(pop)
