#pragma once

struct ID3D11Device;
struct ID3D11DeviceContext;

#pragma pack(push, 1)
namespace jc
{
class HDevice_t
{
  public:
    char                 _pad[0x18];
    ID3D11Device*        m_device;
    ID3D11DeviceContext* m_deviceContext;
    char                 _pad2[0xE4];
    int32_t              m_screenWidth;
    int32_t              m_screenHeight;
};
}; // namespace jc
#pragma pack(pop)
