workspace "jc4-console-thing"
  configurations { "Debug", "Release" }
  location "out"
  systemversion "latest"
  toolset "v141"
  language "C++"
  targetdir "out/%{cfg.buildcfg}"
  objdir "out"
  cppdialect "c++17"
  characterset "MBCS"
  architecture "x64"
  defines { "WIN32", "WIN32_LEAN_AND_MEAN" }

  filter "configurations:Debug"
    defines { "DEBUG", "_DEBUG", "_ITERATOR_DEBUG_LEVEL=0" }
    symbols "On"

  filter "configurations:Release"
    optimize "On"

project "xinput9_1_0"
  kind "SharedLib"
  linkoptions { "/DEF:\"../src/xinput9_1_0.def\"" }
  links { "fw1", "meow-hook" }
  defines { "NOMINMAX" }

  files {
    "src/**.h",
    "src/**.cpp",
    "src/xinput9_1_0.def",
  }

  removefiles "src/generator/**"

  includedirs {
    "src",
    "deps/fw1",
    "deps/meow-hook/include"
  }

  postbuildcommands {
    "copy /Y \"$(TargetDir)$(ProjectName).dll\" \"D:/Steam/steamapps/common/Just Cause 4/$(ProjectName).dll\"",
    "Exit 0"
  }

project "address-generator"
  kind "ConsoleApp"
  links "meow-hook"
  files "src/generator/**"
  includedirs "deps/meow-hook/include"

group "deps"
  project "fw1"
    kind "StaticLib"
    files { "deps/fw1/*.h", "deps/fw1/*.cpp" }

  project "asmjit"
    kind "StaticLib"
    defines "ASMJIT_STATIC"
    files "deps/meow-hook/third_party/asmjit/src/**"
    includedirs "deps/meow-hook/third_party/asmjit/src"

  project "zydis"
    kind "StaticLib"
    defines {
      "ZYCORE_STATIC_DEFINE",
      "ZYDIS_STATIC_DEFINE"
    }

    includedirs {
      "deps/meow-hook/third_party/zydis/src",
      "deps/meow-hook/third_party/zydis/include",
      "deps/meow-hook/third_party/zydis/msvc",
      "deps/meow-hook/third_party/zydis/dependencies/zycore/include"
    }

    files {
      "deps/meow-hook/third_party/zydis/dependencies/zycore/src/**",
      "deps/meow-hook/third_party/zydis/dependencies/zycore/include/**",
      "deps/meow-hook/third_party/zydis/src/**",
      "deps/meow-hook/third_party/zydis/include/Zydis/**",
    }

  project "meow-hook"
    kind "StaticLib"

    links { "asmjit", "zydis" }

    defines {
      "NOMINMAX",
      "ASMJIT_STATIC",
      "ZYCORE_STATIC_DEFINE",
      "ZYDIS_STATIC_DEFINE"
    }

    includedirs {
      "deps/meow-hook/include",
      "deps/meow-hook/third_party/asmjit/src",
      "deps/meow-hook/third_party/zydis/src",
      "deps/meow-hook/third_party/zydis/include",
      "deps/meow-hook/third_party/zydis/msvc",
      "deps/meow-hook/third_party/zydis/dependencies/zycore/include"
    }

    files {
      "deps/meow-hook/src/*.cc",
      "deps/meow-hook/src/*.h",
      "deps/meow-hook/include/meow_hook/*.h",
    }