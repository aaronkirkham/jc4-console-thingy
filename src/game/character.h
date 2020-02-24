#pragma once

#include "../vector.h"
#include "game_object.h"

#include "hooking/hooking.h"

#include <functional>
#include <memory>
#include <vector>

#pragma pack(push, 1)
namespace jc
{
struct CModelRenderBlock {
    char _pad[0x10];
    struct {
        char     _pad[0x108];
        int16_t* m_skeletonLookup;
        uint16_t m_skeletonLookupSize;
    } * m_mesh;
};

struct CAnimatedModel {
    struct CSkinningPaletteData {
        char _pad[0x98];
    };

    char _pad[0xE8];
    struct {
        char                                  _pad[0xD0];
        std::shared_ptr<CSkinningPaletteData> m_skinningPalette;
    } * m_animationController;
    char _pad2[0xD0];
};
static_assert(sizeof(CAnimatedModel) == 0x1C0);

struct CModel {
    char                _pad[0x18];
    CModelRenderBlock** m_renderBlocks;
    uint16_t            m_totalRenderBlockCount;
};

struct CModelInstance {
    char m_rbiInfo[0xC0];
    char _pad[0x18];
    struct {
        struct {
            CModel* m_ptr;
        } * m_handle;
    } m_model;
};
static_assert(offsetof(CModelInstance, m_model) == 0xD8);

struct SPartialModelState {
    struct SModelState {
        char            _pad[0x30];
        CModelInstance* m_modelInstance;
    };

    SModelState m_slot[4];
    uint64_t    m_modelCount;
    char        _pad[0x28];

    void Reset()
    {
        hk::func_call<void>(0x14026B460, this);
    }

    void SetModel(const uint32_t index, const uint32_t name_hash, const CMatrix4f& transform,
                  std::shared_ptr<CAnimatedModel::CSkinningPaletteData>* skinning_palette)
    {
        hk::func_call<void>(0x140267B70, this, index, name_hash, &transform, skinning_palette);
        ++m_modelCount;
    }
};
static_assert(sizeof(SPartialModelState::SModelState) == 0x38);
static_assert(sizeof(SPartialModelState) == 0x110);

class CSharedString;
class CCharacter
{
  public:
    void ChangeSkin(CSharedString& model_name, std::function<void()> callback = nullptr);

  public:
    char               _pad[0x114];
    CMatrix4f          m_worldTransform; // 114 - 154
    char               _pad2[0x484];
    SPartialModelState m_modelState; // 5d8 - 6e8
    char               _pad3[0x1580];
    CAnimatedModel     m_animatedModel; // 1c68 - 1aa8
    char               _pad4[0x460];
    bool               m_unlimitedAmmo; // 2288
};
static_assert(offsetof(CCharacter, m_modelState) == 0x5D8);
static_assert(offsetof(CCharacter, m_animatedModel) == 0x1C68);
static_assert(offsetof(CCharacter, m_unlimitedAmmo) == 0x2288);
}; // namespace jc
#pragma pack(pop)
