#pragma once

#include "../log.hpp"

#ifdef __SPDLOG_IMPLEMENTATION__
#include <spdlog/spdlog.h>
#endif

namespace rainbow::core::logs {

#ifdef __SPDLOG_IMPLEMENTATION__

	template <typename... Args>
	void debug(const std::string& format, const Args& ... args)
	{
		spdlog::debug(format.c_str(), args...);
	}

	template <typename ... Args>
	void info(const std::string& format, const Args&... args)
	{
		spdlog::info(format.c_str(), args...);
	}

	template <typename ... Args>
	void warn(const std::string& format, const Args&... args)
	{
		spdlog::warn(format.c_str(), args...);
	}

	template <typename ... Args>
	void error(const std::string& format, const Args&... args)
	{
		spdlog::error(format, args...);
	}

#else

	template <typename... Args>
	void debug(const std::string& format, const Args& ... args)
	{
	}

	template <typename ... Args>
	void info(const std::string& format, const Args&... args)
	{
	}

	template <typename ... Args>
	void warn(const std::string& format, const Args&... args)
	{
	}

	template <typename ... Args>
	void error(const std::string& format, const Args&... args)
	{
	}

#endif
}