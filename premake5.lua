workspace "Game-Engine"
	architecture "x86_64"
	toolset "v143"
	cppdialect "C++20"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"

	targetdir("../output/bin/" .. outputdir .. "/%{prj.name}")
	objdir("../output/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "stdafx.h"
    pchsource "Engine/src/Headers/stdafx.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
		"premake5.lua"
	}

	includedirs
	{
		"%{prj.name}/src/Actors",
		"%{prj.name}/src/Animation",
		"%{prj.name}/src/Components",
		"%{prj.name}/src/Core",
		"%{prj.name}/src/DataTypes",
		"%{prj.name}/src/Headers",
		"%{prj.name}/src/Physics",
		"%{prj.name}/src/ResourceTypes",
		"%{prj.name}/src/Systems",
		"%{prj.name}/src/Tools",
		"%{prj.name}/src/UI",
		"%{prj.name}/src/World",
		"%{prj.name}/src/Event",
		"%{prj.name}/src/Scene",
		"%{prj.name}/src/Sound",
		"%{prj.name}/src/Input",
		"%{prj.name}/src/Managers",
		"%{prj.name}/vendor/spdlog/include",
		"../SDK/DirectXTK/include",
		"../SDK/FBXSDK/include",
		"../SDK/FMOD/include",
		"../SDK/IMGUI/include",
	}

	libdirs
	{
		"../SDK/DirectXTK/lib",
		"../SDK/IMGUI/lib"
	}

	links
	{
		"d3d11",
		"d3dcompiler",
		"dxgi",
		"dinput8",
		"dxguid",
		"fmod_vc",
		"fmodL_vc",
	}

	filter "system:windows"
		staticruntime "off"
		systemversion "latest"

	filter "configurations:Debug"
		defines "_DEBUG"
		staticruntime "off"
		symbols "On"
		runtime "Debug"

		defines
		{
			"PLATFORM_WINDOWS",
			"BUILD_DLL",
			"_ITERATOR__DEBUGLEVEL=2"
		}

		libdirs
		{
			"../SDK/FBXSDK/lib/debug",
			"../SDK/FMOD/lib/debug",
		}

		links
		{
			"DirectXTK_D",
			"libfbxsdk-md",
			"libxml2-md",
			"zlib-md",
			"ImGui_Win32_Dx11_D",
		}

	filter "configurations:Release"
		defines "_RELEASE"
		staticruntime "off"
		optimize "On"
		runtime "Release"
		

		defines
		{
			"PLATFORM_WINDOWS",
			"BUILD_DLL",
			"_ITERATOR__DEBUGLEVEL=0"
		}


		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
		}

		links
		{
			"DirectXTK"
		}

	filter "configurations:Dist"
		defines "_DIST"
		staticruntime "off"
		optimize "On"
		runtime "Release"

		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
		}
		
		links
		{
			"DirectXTK"
		}