#include <meow_hook/pattern_search.h>

#include <Windows.h>

#include <assert.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

#pragma warning(disable : 4477)

using FindPatternResult = std::vector<std::pair<std::string, intptr_t>>;

static uintptr_t RebaseFileOffset(std::string_view game_file, uintptr_t file_offset)
{
    auto executable_address = reinterpret_cast<intptr_t>(game_file.data());

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)(executable_address);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        throw std::runtime_error("Invalid DOS Signature");
    }

    PIMAGE_NT_HEADERS header = (PIMAGE_NT_HEADERS)(((char*)executable_address + (dosHeader->e_lfanew * sizeof(char))));
    if (header->Signature != IMAGE_NT_SIGNATURE) {
        throw std::runtime_error("Invalid NT Signature");
    }

    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(header);

    for (int32_t i = 0; i < header->FileHeader.NumberOfSections; i++, section++) {
        bool executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
        bool readable   = (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;

        if (file_offset >= section->PointerToRawData
            && file_offset <= (section->PointerToRawData + section->SizeOfRawData)) {
            return uint64_t(0x140000000) + file_offset + ((section->VirtualAddress - section->PointerToRawData));
        }
    }

    std::cout << "Failed to rebase.\n";
    return 0;
}

void FindPattern(const char* name, FindPatternResult& result, std::function<uintptr_t()> func)
{
    uintptr_t addr = 0;

    try {
        addr = func();
    } catch (...) {
        std::cout << "ERROR! ";
    }

    result.push_back({name, addr});
    std::cout << std::setw(40) << std::left << name << std::right << "0x" << std::hex << addr << "\n";
}

intptr_t rebase(std::string_view game_file, intptr_t addr)
{
    return RebaseFileOffset(game_file, addr - reinterpret_cast<intptr_t>(game_file.data()));
}

// HACK: work around for pattern::match::add_disp
meow_hook::pattern::match disp_rebase(std::string_view game_file, meow_hook::pattern::match& match)
{
    auto addr = RebaseFileOffset(game_file, match.addr() - reinterpret_cast<intptr_t>(game_file.data()));
    return meow_hook::pattern::match{addr + *reinterpret_cast<int32_t*>(match.addr())}.adjust(4);
}

FindPatternResult Generate(const char* name, const char* exepath)
{
    using namespace meow_hook;

    FindPatternResult result;

    std::fstream is(exepath, std::ios::in | std::ios::binary);
    is.seekg(0, std::ios::end);
    size_t data_size = is.tellg();
    is.seekg(0, std::ios::beg);
    std::unique_ptr<char[]> data(new char[data_size]);
    is.read(data.get(), data_size);

    std::string_view game_file(data.get(), data_size);

    std::cout << name << "\n";
    std::cout << "-----------\n";

    // clang-format off
    FindPattern("SANITY_CHECK", result, [&] {
        auto match = pattern("48 8D 05 ? ? ? ? 48 89 87 ? ? ? ? 48 89 AF ? ? ? ? C7 87 ? ? ? ? ? ? ? ?", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_CLOCK", result, [&] {
        auto match = pattern("E8 ? ? ? ? 84 C0 74 05 0F 28 F7", game_file).count(1).get(0).adjust(-4);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_CHARACTER_MANAGER", result, [&] {
        auto match = pattern("48 8B 05 ? ? ? ? 48 85 C0 74 08 48 8B 80 ? ? ? ?", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_GAME_WORLD", result, [&] {
        auto match = pattern("48 8B 05 ? ? ? ? 48 85 C0 48 8D 48 F0", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_INPUT_MANAGER", result, [&] {
        auto match = pattern("48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8D 45 A8", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_PLAYER_MANAGER", result, [&] {
        auto match = pattern("48 8B D9 48 8B 05 ? ? ? ? 48 8B 78 30", game_file).count(1).get(0).adjust(6);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_RENDER_ENGINE", result, [&] {
        auto match = pattern("74 63 48 8B 05 ? ? ? ? 48 8B 88 ? ? ? ?", game_file).count(1).get(0).adjust(5);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_SPAWN_SYSTEM", result, [&] {
        auto match = pattern("F3 0F 58 F0 48 8B 05 ? ? ? ?", game_file).count(1).get(0).adjust(7);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_UI_MANAGER", result, [&] {
        auto match = pattern("89 47 54 48 8B 05 ? ? ? ?", game_file).count(1).get(0).adjust(6);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_HNPKWORLD", result, [&] {
        auto match = pattern("48 8B 1D ? ? ? ? 41 8B D6", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INST_WORLDTIME", result, [&] {
        auto match = pattern("73 16 48 8B 05 ? ? ? ?", game_file).count(1).get(0).adjust(5);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("VAR_QUICK_START", result, [&] {
        auto match = pattern("44 38 3D ? ? ? ? 0F 85 ? ? ? ? 48 8B 0D ? ? ? ? 48 85 C9", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("IS_INTRO_SEQUENCE_COMPLETE", result, [&] {
        auto match = pattern("E8 ? ? ? ? 84 C0 74 0A B9 ? ? ? ?", game_file).count(2).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("IS_INTRO_MOVIE_COMPLETE", result, [&] {
        auto match = pattern("E8 ? ? ? ? 84 C0 75 17 48 8B 15 ? ? ? ?", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("WND_PROC", result, [&] {
        auto match = pattern("FF 50 10 48 C7 45 ? ? ? ? ?", game_file).count(1).get(0).adjust(14);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("GRAPHICS_FLIP", result, [&] {
        auto match = pattern("48 8B 8B ? ? ? ? E8 ? ? ? ? 90 48 8B 05 ? ? ? ?", game_file).count(1).get(0).adjust(7).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("PLAYER_MANAGER_UPDATE", result, [&] {
        auto match = pattern("48 83 EC 28 48 8B 51 30", game_file).count(1).get(0).as<uintptr_t>();
        return rebase(game_file, match);
    });

    FindPattern("FIRING_MODULE_CONSUME_AMMO", result, [&] {
        auto match = pattern("41 54 41 56 41 57 48 83 EC 40 48 C7 44 24 ? ? ? ? ? 48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 45 8B E0", game_file).count(1).get(0).as<uintptr_t>();
        return rebase(game_file, match);
    });

    FindPattern("VAR_GAME_STATE", result, [&] {
        auto match = pattern("83 3D ? ? ? ? ? 74 07 33 C9", game_file).count(1).get(0).adjust(2);
        return disp_rebase(game_file, match).adjust(1).as<uintptr_t>();
    });

    FindPattern("VAR_SUSPEND_GAME", result, [&] {
        auto match = pattern("40 38 35 ? ? ? ? 75 53", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("ALLOC", result, [&] {
        auto match = pattern("8D 4D 30 E8 ? ? ? ? 48 8B F8 48 89 44 24 ?", game_file).get(0).adjust(3).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("FREE", result, [&] {
        auto match = pattern("E8 ? ? ? ? 4C 3B 77 08", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("GET_DEFAULT_PLATFORM_ALLOCATOR", result, [&] {
        auto match = pattern("49 89 AF ? ? ? ? E8 ? ? ? ?", game_file).count(1).get(0).adjust(7).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("PLATFORM_ALLOCATOR_FREE", result, [&] {
        auto match = pattern("48 FF 61 08", game_file).count(1).get(0).as<uintptr_t>();
        return rebase(game_file, match);
    });

    FindPattern("CHARACTER_MODEL_COPY_PARTS", result, [&] {
        auto match = pattern("48 89 F2 E8 ? ? ? ? 4D 8B 8D ? ? ? ?", game_file).count(1).get(0).adjust(4);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("CHARACTER_MODEL_SET_PROPERTIES", result, [&] {
        auto match = pattern("48 8B D6 E8 ? ? ? ? 48 8B 4D 17", game_file).count(1).get(0).adjust(3).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("CHARACTER_MODEL_REBUILD_MODEL", result, [&] {
        auto match = pattern("4C 8B C6 E8 ? ? ? ? 33 DB", game_file).count(1).get(0).adjust(3).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("CHARACTER_GET_VEHICLE_PTR", result, [&] {
        auto match = pattern("E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 44 89 2F", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("SEND_EVENT", result, [&] {
        auto match = pattern("E8 ? ? ? ? 49 63 3E", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("SPAWN_SYSTEM_SPAWN", result, [&] {
        auto match = pattern("E8 ? ? ? ? E9 ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 48 8B 08", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("SPAWN_SYSTEM_PARSE_TAGS", result, [&] {
        auto match = pattern("55 56 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 31 E0 48 89 84 24 ? ? ? ?", game_file).count(1).get(0).as<uintptr_t>();
        return rebase(game_file, match);
    });

    FindPattern("SPAWN_SYSTEM_GET_MATCHING_RESOURCES", result, [&] {
        auto match = pattern("E8 ? ? ? ? 48 8B 7C 24 ? 85 C0", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("WORLDTIME_SET_TIME", result, [&] {
        auto match = pattern("E9 ? ? ? ? 0F 2F DA", game_file).count(1).get(0).adjust(1);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INPUT_LOST_FOCUS", result, [&] {
        auto match = pattern("E8 ? ? ? ? E9 ? ? ? ? 31 D2 4C 89 F1", game_file).count(1).get(0).adjust(1);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("INPUT_GAIN_FOCUS", result, [&] {
        auto match = pattern("0F 84 ? ? ? ? E8 ? ? ? ? E9 ? ? ? ? 48 8B 0D ? ? ? ?", game_file).count(1).get(0).adjust(7);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("ENTITY_PROVIDER_CTOR", result, [&] {
        auto match = pattern("E8 ? ? ? ? 48 89 BB ? ? ? ? 48 89 BB ? ? ? ? 48 89 BB ? ? ? ? 48 8B C3 48 8B 5C 24 ? 48 83 C4 40", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("ENTITY_PROVIDER_LOAD_RESOURCES", result, [&] {
        auto match = pattern("E8 ? ? ? ? B1 01 80 3D ? ? ? ? ?", game_file).count(1).get(0).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("ENTITY_PROVIDER_UPDATE_INTERNAL", result, [&] {
        auto match = pattern("48 8D 8A ? ? ? ? E8 ? ? ? ? 48 83 C4 28", game_file).count(1).get(0).adjust(8);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("DRAW_SKIN_BATCHES", result, [&] {
        auto match = pattern("E8 ? ? ? ? 48 8B AC 24 ? ? ? ? 48 8B 74 24 ?", game_file).count(1).get(0).adjust(1);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("RUNTIME_CONTAINER_GET_HASH", result, [&] {
        auto match = pattern("48 8B CD E8 ? ? ? ? 33 FF 48 89 7C 24 ?", game_file).count(1).get(0).adjust(3).extract_call();
        return rebase(game_file, match);
    });

    FindPattern("VTABLE_RENDERBLOCKCHARACTER", result, [&] {
        auto match = pattern("48 89 01 48 83 C1 20 4C 8D 0D ? ? ? ? BA ? ? ? ? 44 8D 42 D2", game_file).count(1).get(0).adjust(-4);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("VTABLE_RENDERBLOCKCHARACTERSKIN", result, [&] {
        auto match = pattern("48 8D 05 ? ? ? ? 48 89 03 48 8B C3 48 83 C4 30 5B C3 33 C0 48 83 C4 30 5B C3 CC CC CC CC CC CC CC 40 53 48 83 EC 40", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });

    FindPattern("VTABLE_RENDERBLOCKGENERAL", result, [&] {
        auto match = pattern("48 8D 05 ? ? ? ? 48 89 01 48 83 C1 20 4C 8D 0D ? ? ? ? BA ? ? ? ? 44 8D 42 D4", game_file).count(1).get(0).adjust(3);
        return disp_rebase(game_file, match).as<uintptr_t>();
    });
    // clang-format on

    std::cout << "\n\n";

    return result;
}

void WriteHeader(const std::filesystem::path& path, FindPatternResult& addresses)
{
    std::ofstream stream(path);

    stream << "#pragma once\n\n";
    stream << "#include <cstdint>\n\n";

    stream << "namespace jc\n";
    stream << "{\n";

    stream << "enum Address {\n";
    for (const auto& address : addresses) {
        stream << "    " << address.first << ",\n";
    }
    stream << "    COUNT,\n";
    stream << "};\n\n";

    stream << "void InitAddresses(bool is_steam);\n";
    stream << "uintptr_t GetAddress(Address address);\n";

    stream << "}; // namespace jc";
}

void WriteSource(const std::filesystem::path& path, FindPatternResult& steam_addresses,
                 FindPatternResult& epic_addresses)
{
    std::ofstream stream(path);

    stream << "#include \"addresses.h\"\n\n";

    stream << "namespace jc\n";
    stream << "{\n";
    stream << "static uintptr_t g_Address[Address::COUNT] = {};\n\n";

    stream << "void InitAddresses(bool is_steam)\n";
    stream << "{\n";

    for (int i = 0; i < steam_addresses.size(); ++i) {
        const std::string& name       = steam_addresses[i].first;
        const uintptr_t    steam_addr = steam_addresses[i].second;
        const uintptr_t    epic_addr  = epic_addresses[i].second;

        stream << "    g_Address[" << name << "] = is_steam ? 0x" << std::hex << steam_addr << " : 0x" << std::hex
               << epic_addr << ";\n";
    }

    stream << "}\n\n";

    stream << "uintptr_t GetAddress(Address address)\n";
    stream << "{\n";
    stream << "    return g_Address[address];\n";
    stream << "}\n";

    stream << "}; // namespace jc";
}

int main()
{
    auto steam_addresses = Generate("Steam", R"(D:\Steam\steamapps\common\Just Cause 4\JustCause4.exe)");
    auto epic_addresses  = Generate("Epic Store", R"(D:\Epic Games\JustCause4\JustCause4.exe)");

    assert(steam_addresses.size() == epic_addresses.size());

    char tmp[MAX_PATH] = {0};
    GetModuleFileName(nullptr, tmp, sizeof(tmp));

    std::filesystem::path path(tmp);
    WriteHeader(path.parent_path() /= "../../src/addresses.h", steam_addresses);
    WriteSource(path.parent_path() /= "../../src/addresses.cpp", steam_addresses, epic_addresses);
    return 0;
}
