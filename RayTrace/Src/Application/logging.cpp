#include "pch.h"

#include "logging.h"

// Define static loggers
std::shared_ptr<spdlog::logger> Logger::s_appLogger;
std::shared_ptr<spdlog::logger> Logger::s_valLogger;

void Logger::init(LogLevel level)
{
	// Initialize loggers
	spdlog::set_pattern("%^[%T] %n: %v%$");

	s_appLogger = spdlog::stdout_color_mt("APP");
	s_appLogger->set_level((spdlog::level::level_enum)level);

	s_valLogger = spdlog::stdout_color_mt("VAL");
	s_valLogger->set_level((spdlog::level::level_enum)level);
}
