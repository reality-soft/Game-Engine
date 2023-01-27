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
		"premake5.lua"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"../SDK/DirectXTK/include",
		"../SDK/FBXSDK/include"
	}

	libdirs
	{
		"../SDK/DirectXTK/lib",
		"../SDK/FBXSDK/lib/debug"
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
		"zlib-md"
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
		"%{prj.name}/src/**.cpp"
	} 

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"../SDK/DirectXTK/include",
		"../SDK/FBXSDK/include"
	}

	libdirs
	{
		"../output/bin/Debug-windows-x86_64/Engine/",
		"../SDK/DirectXTK/lib",
		"../SDK/FBXSDK/lib/debug"
	}

	links
	{
		"Engine",
		"libfbxsdk-md",
		"libxml2-md",
		"zlib-md"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"
		runtime "Debug"

		defines
		{
			"PLATFORM_WINDOWS"
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
