#pragma once

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <unordered_map>

namespace util
{
template <typename... Args> std::string string_format(const std::string &format, Args... args)
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
    if (char *text = static_cast<char *>(GlobalLock(handle))) {
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

static std::wstring ToWideString(const std::string &str)
{
    wchar_t buf[0xfff] = {};
    size_t  converted;
    mbstowcs_s(&converted, buf, str.c_str(), sizeof(buf));
    return std::wstring(buf);
}

class KeyValueFile
{
  public:
    std::string                                  Filename;
    std::unordered_map<std::string, std::string> Values;

    KeyValueFile()  = default;
    ~KeyValueFile() = default;

    void load(const std::string &filename)
    {
        Filename = filename;

        std::ifstream file(filename);
        std::string   line;

        while (file) {
            std::getline(file, line);
            if (line.length() > 0) {
                const auto pos = line.find("\":");
                if (pos != std::string::npos) {
                    auto key   = line.substr(1, pos - 1);
                    auto value = line.substr(pos + 2, line.length());

                    Values[key] = value;
                }
            }
        }
    }

    void save()
    {
        std::ofstream file(Filename);
        for (const auto &entry : Values) {
            file << "\"" << entry.first << "\":" << entry.second << std::endl;
        }
    }

    template <typename... Args> void set(const std::string &key, const std::string &format, Args... args)
    {
        Values[key] = string_format(format, std::forward<Args>(args)...);
        save();
    }

    int32_t get(const std::string &key, const char *format, ...)
    {
        if (Values.find(key) == Values.end()) {
            return 0;
        }

        va_list args;
        va_start(args, format);
        auto result = vsscanf_s(Values[key].c_str(), format, args);
        va_end(args);
        return result;
    }

    void remove(const std::string &key)
    {
        Values.erase(key);
        save();
    }
};
}; // namespace util
