-- Solution
workspace "RayTrace"
	architecture "x64"
	startproject "RayTrace"

	configurations 
	{
		"Debug",   -- Full debugging
		"Release", -- No debugging but logging is still on. Optimization turned on
		"Dist"     -- No debugging or logging. Optimization turned on
	}

-- This is for the output directory. Should look something like "Debug-x86_64"
outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

-- Project
project "RayTrace"
	location "RayTrace"
	kind "ConsoleApp"
	language "C++"

	-- Executable and intermediate objs
	targetdir ( "Bin/" .. outputdir .. "/%{prj.name}" )
	objdir ( "Bin/" .. outputdir .. "/%{prj.name}/Intermediate" )

	-- Source files
	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp",
		"%{prj.name}/Src/**.vert",
		"%{prj.name}/Src/**.frag",
	}

	-- Include directories
	includedirs
	{
		"%{prj.name}/Src",
		"%{prj.name}/Src/Utils",
		"Vendor/VulkanSDK/*/Include",
		"Vendor/GLFW/include",
		"Vendor/GLM",
		"Vendor/spdlog/include",
		"Vendor/tinyobjloader"
	}

	-- Libraries
	libdirs
	{
		"Vendor/VulkanSDK/*/Lib",
		"Vendor/GLFW/lib-vc2022"
	}

	links
	{
		"glfw3",
		"vulkan-1"
	}

	-- PCH
	pchheader "pch.h"
	pchsource "%{prj.name}/Src/Utils/pch.cpp"

	-- Windows
	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

		-- Set the debugging working directory the same as the executable
		debugdir ( "Bin/" .. outputdir .. "/%{prj.name}" )

		-- Warning C4996 is on unsafe or depricated code.
		-- spdlog throws a bunch of these warnings and it clogs up the build output.
		-- For now, this warning will be disabled.
		disablewarnings "4996"

	-- Debug
	filter "configurations:Debug"
		defines "RT_DEBUG"
		symbols "On"

	-- Release
	filter "configurations:Release"
		defines "RT_RELEASE"
		optimize "On"

	-- Dist
	filter "configurations:Dist"
		defines "RT_DIST"
		optimize "On"