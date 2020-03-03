#pragma once

#include "game_object.h"
#include "tarray.h"
#include "vector.h"

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
    char     _pad2[0x18];
    uint32_t m_flags;
};
static_assert(offsetof(CModelInstance, m_model) == 0xD8);
static_assert(offsetof(CModelInstance, m_flags) == 0xF8);

struct SPartialModelData {
  public:
    SPartialModelData()
    {
        m_skinTintData = (void*)(*(uintptr_t*)0x142CB1D40 + 0x270);
    }

    struct SModelEntry {
        uint32_t m_modelHash;
        float    m_weight;
        uint32_t m_skinTintSetting;
    };

    struct STintEntry {
        CVector3f m_colours[3];
        float     m_weight;
    };

    struct SModelSlot {
        float                    m_modelsWeightSum;
        float                    m_tintsWeightSum;
        std::vector<SModelEntry> m_models;
        std::vector<STintEntry>  m_tints;
    };

  public:
    TArray<SModelSlot> m_slots;
    void*              m_skinTintData;
};
static_assert(sizeof(SPartialModelData::SModelEntry) == 0xC);
static_assert(sizeof(SPartialModelData::STintEntry) == 0x28);
static_assert(sizeof(SPartialModelData::SModelSlot) == 0x38);
static_assert(sizeof(SPartialModelData) == 0x28);

struct SPartialModelState {
    struct SModelState {
        char            _pad[0x30];
        CModelInstance* m_modelInstance;
    };

    SModelState m_slot[4];
    uint64_t    m_modelCount;
    char        _pad[0x28];
};
static_assert(sizeof(SPartialModelState::SModelState) == 0x38);
static_assert(sizeof(SPartialModelState) == 0x110);

class CSkinSwapper : public CGameObject
{
  public:
    char               _pad[0x1C8];
    uint32_t           m_requestedSkin;
    uint32_t           m_currentSkin;
    SPartialModelState m_oldModelState;
    uint32_t           m_defaultSkin;
};
static_assert(offsetof(CSkinSwapper, m_requestedSkin) == 0x1d0);
static_assert(offsetof(CSkinSwapper, m_defaultSkin) == 0x2e8);

class CSharedString;
class CCharacter
{
  public:
    void ChangeSkin(CSharedString& model_name, std::function<void()> callback = nullptr);

    CGameObject* GetVehiclePtr()
    {
        return hk::func_call<CGameObject*>(0x14054FD50, this);
    }

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
