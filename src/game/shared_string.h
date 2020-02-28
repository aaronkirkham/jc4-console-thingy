#pragma once

#include <memory>
#include <string>

namespace jc
{
class CSharedString
{
  public:
    std::shared_ptr<std::basic_string<char, std::char_traits<char>, std::allocator<char>> const> m_str;
};
static_assert(sizeof(CSharedString) == 0x10);
} // namespace jc
