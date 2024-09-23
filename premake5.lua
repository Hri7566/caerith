workspace "caerith"
    configurations { "Debug", "Release" }

project "caerith"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/${cfg.buildcfg}"
    links { "raylib", "m", "rtmidi" }

    files {
        "include/**.h",
        "src/**.h",
        "src/**.c",
        "include/**.hpp",
        "src/**.hpp",
        "src/**.cpp"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
