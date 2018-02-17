require("premake", ">=5.0.0-alpha12")

solution "Test"
   location "generated"
   configurations { "Debug", "Release" }

project "Test"
   kind "ConsoleApp"
   sysincludedirs {
     "usr/local/include"
   }


../usr/local/include