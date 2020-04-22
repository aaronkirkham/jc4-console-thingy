#pragma once

#pragma pack(push, 1)
namespace jc::Base
{
class CClock
{
  public:
    static CClock& instance()
    {
        return **(CClock**)GetAddress(INST_CLOCK);
    }

  public:
    char _pad[0x30];
    bool m_paused;
};
}; // namespace jc::Base
#pragma pack(pop)
