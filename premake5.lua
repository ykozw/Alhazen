require("premake", ">=5.0.0-alpha12")

solution "Alhazen"
   location "generated"
   configurations { "Debug", "Release", "Ship" }
   platforms {"x64"}

-- global scope

kind "ConsoleApp"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
vectorextensions "AVX2"

pchsource "src/pch.cpp"
pchheader "pch.hpp"
forceincludes  { "pch.hpp" }

includedirs {
  "src",
  "thirdparty/tinyxml2",
  "thirdparty/stb",
  "thirdparty/vdb",
  "thirdparty/tinyobjloader",
  "thirdparty/catch2/single_include",
  "%EMBREE_ROOT%/include/",
  "$(EMBREE_ROOT)/include/",
}

files {
  "src/pch.cpp",
  "src/**.hpp",
  "src/**.cpp",
  "src/**.inl", 
  "thirdparty/tinyxml2/tinyxml2.cpp"
}

characterset "MBCS"

-- "cppdialect"は現在壊れているので直接buildoptionsを指定する
configuration "windows"
  cppdialect "C++17"
configuration "macosx"
  buildoptions {"-std=c++14"}

filter "action:not vs*"
   pchheader "../src/pch.hpp"

filter "action: vs*"
  pchsource "src/pch.cpp"
  pchheader "pch.hpp"

filter "files:thirdparty/tinyxml2/tinyxml2.cpp"
  flags { "NoPCH" }
 
-- 全体デバッグ用
configuration "Debug"
  defines { "DEBUG" }
  symbols "On"

-- 通常開発時用。デバッグ時はOPT_OFFを適宜つける
configuration "Release"
  defines { "NDEBUG", "NO_ASSERT" }
  symbols "On"
  optimize "On"

-- 最終的なバイナリ出力時用。2割以上高速なのでClangを利用する。
configuration "Ship"
  defines { "NDEBUG", "NO_ASSERT" }
  optimize "Speed"
  flags { "LinkTimeOptimization", "NoRuntimeChecks", "StaticRuntime" }
  toolset "msc-llvm-vs2014"
  -- LLVMを使ってもZ7でデバッグ情報を生成するとデバッグが可能になる
  -- buildoptions { "/Z7" }

configuration "macosx"
  sysincludedirs {
    "/usr/local/include"
  }
  libdirs {
    "/usr/local/lib"
  }
  links {
    "tbb",
    "tbbmalloc_proxy",
    "tbbmalloc"
  }

configuration "windows"
  sysincludedirs {
    "thirdparty/tbb/include",
    "$(EMBREE_ROOT)include/",
  }
  libdirs {
    "thirdparty/tbb/lib/intel64/vc14",
    "$(EMBREE_ROOT)lib/",
  }
  links {
    "tbb.lib",
    "tbbmalloc_proxy.lib",
    "tbbmalloc.lib",
    "embree3.lib",
  }

-- projects
project "Alhazen"
  location "generated/alhazen"

project "Alhazen_test"
  location "generated/test"
  excludes {
    "src/app/*.*"
  }
  files {
    "tests/*.cpp",
  }
