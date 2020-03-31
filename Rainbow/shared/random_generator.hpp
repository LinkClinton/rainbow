#pragma once

#include "../interfaces/noncopyable.hpp"

#include "utilities.hpp"

#include <random>

namespace rainbow {

	class random_generator : public interface::noncopyable {
	public:
		random_generator();

		~random_generator() = default;
		
		explicit random_generator(const uint64 seed);

		real normalize_real();
		
		real real(const real min = 0, const real max = 1);
	private:
		std::default_random_engine mEngine;
		
		std::uniform_real_distribution<rainbow::real> mDistribution;
	};
	
}
