#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class Logger
{
public:
	void init(spdlog::level::level_enum logLevel);

	std::shared_ptr<spdlog::logger> getLogger() const;

private:
	std::shared_ptr<spdlog::logger> m_logger;
};

// Logging macros
// * EVERY LOGGER MUST BE NAME "m_logger" *
#ifndef RT_DIST
	#define LOG_TRACE(...)    m_logger.getLogger()->trace(__VA_ARGS__)
	#define LOG_INFO(...)     m_logger.getLogger()->info(__VA_ARGS__)
	#define LOG_WARN(...)     m_logger.getLogger()->warn(__VA_ARGS__)
	#define LOG_ERROR(...)    m_logger.getLogger()->error(__VA_ARGS__)
	#define LOG_CRITICAL(...) m_logger.getLogger()->critical(__VA_ARGS__)
#else
	#define LOG_TRACE(...)    (void)0
	#define LOG_INFO(...)     (void)0
	#define LOG_WARN(...)     (void)0
	#define LOG_ERROR(...)    (void)0
	#define LOG_CRITICAL(...) (void)0
#endif