#pragma once

#include <memory>

#include "DllMacro.h"
#include "spdlog/spdlog.h"

namespace reality {
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
#define ENGINE_TRACE(...)		::reality::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...)		::reality::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)		::reality::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...)		::reality::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...)	::reality::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Core log macros
#define CLIENT_TRACE(...)		::reality::Log::GetClientLogger()->trace(__VA_ARGS__)
#define CLIENT_INFO(...)		::reality::Log::GetClientLogger()->info(__VA_ARGS__)
#define CLIENT_WARN(...)		::reality::Log::GetClientLogger()->warn(__VA_ARGS__)
#define CLIENT_ERROR(...)		::reality::Log::GetClientLogger()->error(__VA_ARGS__)
#define CLIENT_CRITICAL(...)	::reality::Log::GetClientLogger()->critical(__VA_ARGS__)