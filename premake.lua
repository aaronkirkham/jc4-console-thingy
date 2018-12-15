workspace "jc4-console-thing"
  configurations { "Debug", "Release" }
  location "out"
  systemversion "latest"
  language "C++"
  targetdir "out/%{cfg.buildcfg}"
  objdir "out"
  cppdialect "c++17"
  characterset "MBCS"
  architecture "x64"
  disablewarnings { "4005", "26451", "26491", "26495", "28020" }
  defines { "WIN32", "WIN32_LEAN_AND_MEAN", "NOMINMAX", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE" }

  filter "configurations:Debug"
    defines { "DEBUG", "_DEBUG", "_ITERATOR_DEBUG_LEVEL=0" }
    symbols "On"

  filter "configurations:Release"
    optimize "On"

project "xinput9_1_0"
  kind "SharedLib"
  linkoptions { "/DEF:\"../src/xinput9_1_0.def\"" }
  dependson { "fw1" }
  links { "out/%{cfg.buildcfg}/fw1" }
  files {
    "src/**.h",
    "src/**.cpp",
    "src/xinput9_1_0.def",
    "deps/libudis86/*.c",
    "deps/libudis86/*.h",
  }

  includedirs {
    "src",
    "deps/libudis86",
    "deps/fw1/FW1FontWrapper/Source"
  }

  disablewarnings { "4244", "4267", "4996", "6031", "6262" }

project "fw1"
  kind "StaticLib"
  files { "deps/fw1/FW1FontWrapper/Source/*.h", "deps/fw1/FW1FontWrapper/Source/*.cpp" }
  includedirs { "deps/fw1/FW1FontWrapper/Source/" }