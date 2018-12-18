#pragma once

#include <string>

namespace util
{
template <typename... Args> std::string string_format(const std::string& format, Args... args)
{
    size_t                  size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

static std::string GetClipboard(uint32_t max_length = 255)
{
    if (!OpenClipboard(nullptr)) {
        return "";
    }

    auto handle = GetClipboardData(CF_TEXT);
    if (handle == nullptr) {
        CloseClipboard();
        return "";
    }

    std::string result;
    if (char* text = static_cast<char*>(GlobalLock(handle))) {
        result = text;

        // strip characters
        result.erase(std::remove_if(result.begin(), result.end(),
                                    [](char c) { return (c == '\r' || c == '\n' || c == '\t' || c == '\v'); }),
                     result.end());

        if (result.length() > max_length) {
            result.resize(max_length);
        }
    }

    GlobalUnlock(handle);
    CloseClipboard();

    return result;
}
}; // namespace util
