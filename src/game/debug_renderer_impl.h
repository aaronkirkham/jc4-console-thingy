#pragma once

#include <cstdint>

#include "../vector.h"

#pragma pack(push, 1)
namespace jc
{
class DebugRendererImpl
{
  public:
    virtual ~DebugRendererImpl()                                                                               = 0;
    virtual void DebugLine3d()                                                                                 = 0;
    virtual void DebugLine(const CVector2f &from, const CVector2f &to, uint32_t from_color, uint32_t to_color) = 0;
    virtual void function_4()                                                                                  = 0;
    virtual void function_5()                                                                                  = 0;
    virtual void DebugTriangle(const CVector2f &p0, const CVector2f &p1, const CVector2f &p2, uint32_t c0, uint32_t c1,
                               uint32_t c2)                                                                    = 0;
    virtual void DebugTriangleFilled(const CVector2f &p0, const CVector2f &p1, const CVector2f &p2, uint32_t c0,
                                     uint32_t c1, uint32_t c2)                                                 = 0;
    virtual void function_8()                                                                                  = 0;
    virtual void function_9()                                                                                  = 0;
    virtual void function_10()                                                                                 = 0;
    virtual void function_11()                                                                                 = 0;
    virtual void function_12()                                                                                 = 0;
    virtual void function_13()                                                                                 = 0;
    virtual void function_14()                                                                                 = 0;
    virtual void function_15()                                                                                 = 0;
    virtual void function_16()                                                                                 = 0;
    virtual void function_17()                                                                                 = 0;
    virtual void function_18()                                                                                 = 0;
    virtual void function_19()                                                                                 = 0;
    virtual void function_20()                                                                                 = 0;
    virtual void function_21()                                                                                 = 0;
    virtual void function_22()                                                                                 = 0;
    virtual void function_23()                                                                                 = 0;
    virtual void function_24()                                                                                 = 0;
    virtual void function_25()                                                                                 = 0;
    virtual void function_26()                                                                                 = 0;
    virtual void DebugRect(const CVector2f &topleft, const CVector2f &bottomright, uint32_t color)             = 0;
    virtual void DebugRectFilled(const CVector2f &topleft, const CVector2f &bottomright, uint32_t color)       = 0;
};
}; // namespace jc
#pragma pack(pop)
