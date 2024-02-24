project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}/Intermediate")

	files
	{
        "Src/**.cpp",
        "Src/**.h"
	}

	-- Include directories
	includedirs
	{
		"%{wks.location}/Vendor/VulkanSDK/*/Include",
		"%{wks.location}/Vendor/GLFW/include"
	}

	-- Libraries
	libdirs
	{
		"%{wks.location}/Vendor/VulkanSDK/*/Lib",
		"%{wks.location}/Vendor/GLFW/lib-vc2022"
	}

	links
	{
		"glfw3",
		"vulkan-1"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"