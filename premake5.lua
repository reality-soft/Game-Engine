workspace "Game-Engine"
	architecture "x86_64"
	toolset "v143"
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

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
		"premake5.lua"
	}

	includedirs
	{
		"%{prj.name}/src/Engine",
		"%{prj.name}/src/Engine/SingletonClass",
		"%{prj.name}/src/ECS",
		"%{prj.name}/vendor/spdlog/include",
		"../SDK/DirectXTK/include",
		"../SDK/FBXSDK/include",
		"../SDK/FMOD/include"
	}

	libdirs
	{
		"../SDK/DirectXTK/lib",
		"../SDK/FBXSDK/lib/debug",
		"../SDK/FMOD/lib/debug"
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
		"fmodL_vc"
	}

	filter "system:windows"
		cppdialect "C++17"
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
			("{COPY} %{cfg.buildtarget.relpath} ../../output/bin/" .. outputdir .. "/TestGame")
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "_RELEASE"
		optimize "On"
	filter "configurations:Dist"
		defines "_DIST"
		optimize "On"


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
		"%{prj.name}/Shader/**.hlsl"
	} 

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"Engine/src/Engine",
		"Engine/src/Engine/SingletonClass",
		"Engine/src/ECS",
		"../SDK/DirectXTK/include",
		"../SDK/FBXSDK/include",
		"../SDK/FMOD/include"
	}

	libdirs
	{
		"../SDK/DirectXTK/lib",
		"../SDK/FBXSDK/lib/debug",
		"../SDK/FMOD/lib/debug"
	}

	links
	{
		"Engine",
		"libfbxsdk-md",
		"libxml2-md",
		"zlib-md",
		"fmod_vc",
		"fmodL_vc"
	}

	filter "files:**VS.hlsl"
		shadertype "Vertex"
		shaderentry "VS"
	    shadermodel "5.0"

	filter "files:**PS.hlsl"
	    shadertype "Pixel"
		shaderentry "PS"
	    shadermodel "5.0"

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"
		runtime "Debug"

		defines
		{
			"PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
			"copy \"..\\..\\output\\bin\\Debug-windows-x86_64\\TestGame\\*.cso\" \"Shader\\*.cso\""
		}

	filter "configurations:Debug"
		defines "_DEBUG"
		symbols "On"
	filter "configurations:Release"
		defines "_RELEASE"
		optimize "On"
	filter "configurations:Dist"
		defines "_DIST"
		optimize "On"
