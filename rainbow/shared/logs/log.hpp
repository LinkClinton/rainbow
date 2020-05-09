#pragma once

#define __SPDLOG_IMPLEMENTATION__

#pragma warning(disable : 4275)

#include <string>

namespace rainbow {

	namespace logs {

		template <typename... Args>
		void debug(const std::string& format, const Args& ... args);

		template <typename... Args>
		void info(const std::string& format, const Args& ... args);

		template <typename... Args>
		void warn(const std::string& format, const Args& ... args);

		template <typename... Args>
		void error(const std::string& format, const Args& ... args);
		
	}
}

#include "detail/log.hpp"