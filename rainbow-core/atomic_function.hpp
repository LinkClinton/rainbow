#pragma once

#include "utilities.hpp"

#include <atomic>

namespace rainbow::core {

	void atomic_real_add(std::atomic<real>& value, real add);
	
}
