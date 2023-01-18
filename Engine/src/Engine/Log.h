#pragma once

#include <memory>

#include "DllMacro.h"
#include "spdlog/spdlog.h"

namespace KGCA41B {
	class DLL_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return core_logger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return client_logger; };
	private:
		static std::shared_ptr<spdlog::logger> core_logger;
		static std::shared_ptr<spdlog::logger> client_logger;
	};
}

// Core log macros
#define ENGINE_TRACE(...)		::KGCA41B::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...)		::KGCA41B::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)		::KGCA41B::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...)		::KGCA41B::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...)	::KGCA41B::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Core log macros
#define CLIENT_TRACE(...)		::KGCA41B::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_INFO(...)		::KGCA41B::Log::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_WARN(...)		::KGCA41B::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_ERROR(...)		::KGCA41B::Log::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_CRITICAL(...)	::KGCA41B::Log::GetClientLogger()->critical(__VA_ARGS__)