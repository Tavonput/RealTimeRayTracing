#include "logging.h"

void Logger::init(spdlog::level::level_enum logLevel)
{
	// Initialize logger
	m_logger = spdlog::stdout_color_mt("LOGGER");
	m_logger->set_pattern("%^[%T] %n: %v%$");
	m_logger->set_level(logLevel);
}

std::shared_ptr<spdlog::logger> Logger::getLogger() const
{
	return m_logger;
}
