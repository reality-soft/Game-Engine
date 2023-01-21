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
		"%{prj.name}/src/**.cpp"
	}

	libdirs
	{
		"../SDK/DirectXTK/lib"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"../SDK/DirectXTK/include"
	}

	links
	{
		"d3d11.lib",
		"d3dcompiler.lib",
		"dxgi.lib",
		"dinput8.lib",
		"dxguid.lib",
		"DirectXTK_D.lib"
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
			("{COPY} %{cfg.buildtarget.relpath} ../TestGame")
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
		"../SDK/DirectXTK/include"
	}

	libdirs
	{

	}

	links
	{
		"Engine",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

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
