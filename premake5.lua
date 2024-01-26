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
		"%{prj.name}/Src/**.cpp"
	}

	-- Include directories
	includedirs
	{
		"%{prj.name}/Src",
		"Vendor/VulkanSDK/*/Include",
		"Vendor/GLFW/include",
		"Vendor/GLM",
		"Vendor/spdlog/include"
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

	-- Windows
	filter "system:windows"
		cppdialect "C++20"
		systemversion "latest"

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