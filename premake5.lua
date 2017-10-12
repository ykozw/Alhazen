solution "Alhazen"
   location "generated"
   configurations { "Debug", "Release", "Ship" }
   platforms {"x64"}

project "Alhazen"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   
   pchsource "src/pch.cpp"
   pchheader "pch.hpp"
   includedirs {
     "src",
     "thirdparty/tinyxml2",
     "thirdparty/stb",
     "thirdparty/vdb",
     -- XCode対策(途中)
     "../src",
     "../../src"
   }

   files {
     "src/pch.cpp",
     "src/**.hpp",
     "src/**.cpp",
     "src/**.inl", 
     "thirdparty/tinyxml2/tinyxml2.cpp"
   }

   characterset "MBCS"
   flags { "C++14" }

   configuration "vs*"
      buildoptions { "/bigobj" }

  filter "action:not vs*"
     pchheader "../src/pch.hpp"

  filter "action: vs*"
    pchsource "src/pch.cpp"
    pchheader "pch.hpp"
  
  filter "files:thirdparty/tinyxml2/tinyxml2.cpp"
    flags { "NoPCH" }
   
  -- 全体デバッグ用
  filter "configurations:Debug"
    defines { "DEBUG" }
    flags { "Symbols" }
  
  -- 通常開発時用。デバッグ時はOPT_OFFを適宜つける
  filter "configurations:Release"
    defines { "NDEBUG", "NO_ASSERT" }
    flags { "Symbols" }
    optimize "On"
  
  -- 最終的なバイナリ出力時用
  filter "configurations:Ship"
    defines { "NDEBUG", "NO_ASSERT" }
    optimize "Speed"
    flags { "LinkTimeOptimization", "NoRuntimeChecks", "StaticRuntime" }
