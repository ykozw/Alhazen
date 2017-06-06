solution "Alhazen"
   location "generated"
   configurations { "Debug", "UnitTest", "Release", "Ship" }
   platforms {"x64"}

project "Alhazen"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   pchsource "src/pch.cpp"
   pchheader "pch.hpp"
   includedirs {
     "src",
     "thirdparty/tinyxml2"
   }
   files {
     "src/pch.cpp",
     "src/**.hpp",
     "src/**.cpp",
     "src/**.inl", 
     "thirdparty/tinyxml2/tinyxml2.cpp"
   }
   characterset "MBCS"
   
   filter "files:thirdparty/tinyxml2/tinyxml2.cpp"
     flags { "NoPCH" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }
  
  filter "configurations:UnitTest"
      defines { "UNIT_TEST", "NDEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      flags { "Symbols" }
      optimize "On"
  
   filter "configurations:Ship"
      defines { "NDEBUG", "NO_ASSERT" }
      optimize "Speed"
      flags { "LinkTimeOptimization", "NoRuntimeChecks", "StaticRuntime" }
