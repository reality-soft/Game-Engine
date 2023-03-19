#pragma once

#ifdef PLATFORM_WINDOWS
	#ifdef BUILD_DLL
		#define DLL_API __declspec(dllexport)
	#else
		#define DLL_API __declspec(dllimport)
	#endif
#else
	#error only supports Windows
#endif