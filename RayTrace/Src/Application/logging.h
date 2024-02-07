#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

enum class LogLevel
{
	TRACE    = spdlog::level::trace,
	INFO     = spdlog::level::info,
	WARN     = spdlog::level::warn,
	ERR      = spdlog::level::err,
	CRTITCAL = spdlog::level::critical
};

class Logger
{
public:
	static void init(LogLevel level);

	static void changeLogLevel(LogLevel level);

	inline static std::shared_ptr<spdlog::logger>& getAppLogger() { return s_appLogger; }
	inline static std::shared_ptr<spdlog::logger>& getValLogger() { return s_valLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_appLogger;
	static std::shared_ptr<spdlog::logger> s_valLogger;
};

// Logging macros
#ifndef RT_DIST
	#define APP_LOG_TRACE(...)    Logger::getAppLogger()->trace(__VA_ARGS__)
	#define APP_LOG_INFO(...)     Logger::getAppLogger()->info(__VA_ARGS__)
	#define APP_LOG_WARN(...)     Logger::getAppLogger()->warn(__VA_ARGS__)
	#define APP_LOG_ERROR(...)    Logger::getAppLogger()->error(__VA_ARGS__)
	#define APP_LOG_CRITICAL(...) Logger::getAppLogger()->critical(__VA_ARGS__)

	#define VAL_LOG_TRACE(...)    Logger::getValLogger()->trace(__VA_ARGS__)
	#define VAL_LOG_INFO(...)     Logger::getValLogger()->info(__VA_ARGS__)
	#define VAL_LOG_WARN(...)     Logger::getValLogger()->warn(__VA_ARGS__)
	#define VAL_LOG_ERROR(...)    Logger::getValLogger()->error(__VA_ARGS__)
	#define VAL_LOG_CRITICAL(...) Logger::getValLogger()->critical(__VA_ARGS__)
#else
	#define APP_LOG_TRACE(...)    (void)0
	#define APP_LOG_INFO(...)     (void)0
	#define APP_LOG_WARN(...)     (void)0
	#define APP_LOG_ERROR(...)    (void)0
	#define APP_LOG_CRITICAL(...) (void)0

	#define VAL_LOG_TRACE(...)    (void)0
	#define VAL_LOG_INFO(...)     (void)0
	#define VAL_LOG_WARN(...)     (void)0
	#define VAL_LOG_ERROR(...)    (void)0
	#define VAL_LOG_CRITICAL(...) (void)0
#endif
