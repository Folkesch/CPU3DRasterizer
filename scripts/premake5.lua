-- premake5.lua
workspace "CPURastericer"
   configurations { "Debug", "Release" }
   platforms { "Win32", "Win64"}
   location "../build/"

project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++latest"
   targetdir "../build/App/bin/%{cfg.buildcfg}"
   location "../build/App"

   files { "../src/**.h", "../src/**.cpp" }

   vpaths {
      ["src/*"] = { "../src/**.cpp" },
      ["src/header/*"] = {"../src/**.h" }
   }

   includedirs { "../vendor" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
   
   filter { "platforms:Win32" }
      system "Windows"
      architecture "x86"
  
   filter { "platforms:Win64" }
      system "Windows"
      architecture "x86_64"