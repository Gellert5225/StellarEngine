#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Stellar {
	class STLR_API Log {
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetEditorConsoleLogger() { return s_EditorConsoleLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
		static std::shared_ptr<spdlog::logger> s_EditorConsoleLogger;
	};
}

// core
#define STLR_EXPAND_VARGS(x) x

#define STLR_CORE_TRACE(...) ::Stellar::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define STLR_CORE_INFO(...)  ::Stellar::Log::GetCoreLogger()->info(__VA_ARGS__)
#define STLR_CORE_WARN(...)  ::Stellar::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define STLR_CORE_ERROR(...) ::Stellar::Log::GetCoreLogger()->error(__VA_ARGS__)
#define STLR_CORE_FATAL(...) ::Stellar::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define STLR_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { STLR_CORE_ERROR("Assertion Failed"); DEBUG_BREAK; } }
#define STLR_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { STLR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; } }

#define STLR_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define STLR_GET_ASSERT_MACRO(...) STLR_EXPAND_VARGS(STLR_ASSERT_RESOLVE(__VA_ARGS__, STLR_ASSERT_MESSAGE, STLR_ASSERT_NO_MESSAGE))

#define STLR_CORE_ASSERT(...) STLR_EXPAND_VARGS( STLR_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )

// client
#define STLR_TRACE(...) ::Stellar::Log::GetClientLogger()->trace(__VA_ARGS__)
#define STLR_INFO(...)  ::Stellar::Log::GetClientLogger()->info(__VA_ARGS__)
#define STLR_WARN(...)  ::Stellar::Log::GetClientLogger()->warn(__VA_ARGS__)
#define STLR_ERROR(...) ::Stellar::Log::GetClientLogger()->error(__VA_ARGS__)
#define STLR_FATAL(...) ::Stellar::Log::GetClientLogger()->critical(__VA_ARGS__)
#define STLR_ASSERT(...) STLR_EXPAND_VARGS( STLR_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )

// Editor Console Logging Macros
#define STLR_CONSOLE_LOG_TRACE(...)   Stellar::Log::GetEditorConsoleLogger()->trace(__VA_ARGS__)
#define STLR_CONSOLE_LOG_INFO(...)    Stellar::Log::GetEditorConsoleLogger()->info(__VA_ARGS__)
#define STLR_CONSOLE_LOG_DEBUG(...)   Stellar::Log::GetEditorConsoleLogger()->debug(__VA_ARGS__)
#define STLR_CONSOLE_LOG_WARN(...)    Stellar::Log::GetEditorConsoleLogger()->warn(__VA_ARGS__)
#define STLR_CONSOLE_LOG_ERROR(...)   Stellar::Log::GetEditorConsoleLogger()->error(__VA_ARGS__)
#define STLR_CONSOLE_LOG_FATAL(...)   Stellar::Log::GetEditorConsoleLogger()->critical(__VA_ARGS__)