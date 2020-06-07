#pragma once

#include "../../rainbow-core/utilities.hpp"

#include "../interfaces/noncopyable.hpp"

#include <random>

namespace rainbow::cpus::shared {

	using namespace core;
	
	class default_random_generator final : public interfaces::noncopyable {
	public:
		default_random_generator();

		~default_random_generator() = default;
		
		explicit default_random_generator(const uint64 seed);

		real uniform_real();
		
		real real(real min = 0, real max = 1);

		uint32 uint32(uint32 min, uint32 max);
	private:
		std::default_random_engine mEngine;
		
		std::uniform_real_distribution<core::real> mDistribution;
	};
	
	class pcg32 final : public interfaces::noncopyable {
	public:
		pcg32();

		~pcg32() = default;

		explicit pcg32(const uint64 seed);
		
		real uniform_real();

		uint32 uniform_uint32();

		real real(real min = 0, real max = 1);
		
		uint32 uint32(uint32 min, uint32 max);
	private:
		core::uint32 uint32(core::uint32 value);
		
		uint64 mState, mInc;
	};
	
	using random_generator = pcg32;
}
