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
		"DirectXTK_D",
		"libfbxsdk-md",
		"libxml2-md",
		"zlib-md",
		"fmod_vc",
		"fmodL_vc",
		"ImGui_Win32_Dx11_D"
	}

	filter "system:windows"
		staticruntime "off"
		systemversion "latest"
		runtime "Debug"

		defines
		{
			"PLATFORM_WINDOWS",
			"BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../../output/bin/" .. outputdir .. "/TestGame"),
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		symbols "On"

		libdirs
		{
			"../SDK/FBXSDK/lib/debug",
			"../SDK/FMOD/lib/debug",
		}

	filter "configurations:Release"
		defines "_RELEASE"
		optimize "On"

		
		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
		}

	filter "configurations:Dist"
		defines "_DIST"
		optimize "On"

		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
		}


project "TestGame"
	location "TestGame"
	kind "WindowedApp"
	language "C++"

	targetdir("../output/bin/" .. outputdir .. "/%{prj.name}")
	objdir("../output/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/HLSL/**.hlsl"
	} 

	includedirs
	{
		"Engine/src",
		"Engine/src/Actors",
		"Engine/src/Components",
		"Engine/src/Core",
		"Engine/src/DataTypes",
		"Engine/src/Headers",
		"Engine/src/Physics",
		"Engine/src/ResourceTypes",
		"Engine/src/Systems",
		"Engine/src/Tools",
		"Engine/src/UI",
		"Engine/src/World",
		"Engine/src/Event",
		"Engine/src/Scene",
		"Engine/src/Sound",
		"Engine/src/Input",
		"Engine/src/Managers",
		"../SDK/DirectXTK/include",
		"../SDK/FBXSDK/include",
		"../SDK/FMOD/include",
		"../SDK/IMGUI/include"
	}

	libdirs
	{
		"../SDK/DirectXTK/lib",
		"../SDK/IMGUI/lib"
	}

	links
	{
		"Engine",
		"libfbxsdk-md",
		"libxml2-md",
		"zlib-md",
		"fmod_vc",
		"fmodL_vc",
		"ImGui_Win32_Dx11_D"
	}

	filter "files:**VS.hlsl"
		shadertype "Vertex"
		shaderentry "VS"
	    shadermodel "5.0"

	filter "files:**PS.hlsl"
	    shadertype "Pixel"
		shaderentry "PS"
	    shadermodel "5.0"
		
	filter "files:**GS.hlsl"
	    shadertype "Geometry"
		shaderentry "GS"
	    shadermodel "5.0"

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		symbols "On"
		runtime "Debug"

		libdirs
		{
			"../SDK/FBXSDK/lib/debug",
			"../SDK/FMOD/lib/debug",
		}

		postbuildcommands
		{
			"copy \"..\\..\\output\\bin\\Debug-windows-x86_64\\TestGame\\*.cso\" \"..\\..\\Contents\\Shader\\*.cso\"",
		}

	filter "configurations:Release"
		defines "_RELEASE"
		optimize "On"
		runtime "Release"

		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
		}

		postbuildcommands
		{
			"copy \"..\\..\\output\\bin\\Release-windows-x86_64\\TestGame\\*.cso\" \"..\\..\\Contents\\Shader\\*.cso\"",
		}

	filter "configurations:Dist"
		defines "_DIST"
		optimize "On"

		libdirs
		{
			"../SDK/FBXSDK/lib/release",
			"../SDK/FMOD/lib/release",
		}

		postbuildcommands
		{
			"copy \"..\\..\\output\\bin\\Release-windows-x86_64\\TestGame\\*.cso\" \"..\\..\\Contents\\Shader\\*.cso\"",
		}