#include "random_generator.hpp"

#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL

#include <ctime>

rainbow::cpus::shared::default_random_generator::default_random_generator() : default_random_generator(static_cast<uint64>(time(nullptr)))
{
	
}

rainbow::cpus::shared::default_random_generator::default_random_generator(const uint64 seed) : mEngine(static_cast<unsigned>(seed)), mDistribution(0, 1)
{
}

rainbow::core::real rainbow::cpus::shared::default_random_generator::uniform_real()
{
	return mDistribution(mEngine);
}

rainbow::core::real rainbow::cpus::shared::default_random_generator::real(core::real min, core::real max)
{
	const auto distribution = std::uniform_real_distribution<core::real>(min, max);

	return distribution(mEngine);
}

rainbow::core::uint32 rainbow::cpus::shared::default_random_generator::uint32(core::uint32 min, core::uint32 max)
{
	const auto distribution = std::uniform_int_distribution<core::uint32>(min, max);

	return distribution(mEngine);
}

rainbow::cpus::shared::pcg32::pcg32() : mState(PCG32_DEFAULT_STATE), mInc(PCG32_DEFAULT_STREAM)
{
}

rainbow::cpus::shared::pcg32::pcg32(const uint64 seed)
{
	mState = 0u;
	mInc = (seed << 1u) | 1u;
	uniform_uint32();
	mState += PCG32_DEFAULT_STATE;
	uniform_uint32();
}

rainbow::core::real rainbow::cpus::shared::pcg32::uniform_real()
{
	return std::min(1 - std::numeric_limits<core::real>::epsilon(),
		static_cast<core::real>(uniform_uint32() * 2.3283064365386963e-10f));
}

rainbow::core::uint32 rainbow::cpus::shared::pcg32::uniform_uint32()
{
	const auto old_state = mState;

	mState = old_state * PCG32_MULT + mInc;

	const auto xor_shifted = static_cast<core::uint32>(((old_state >> 18u) ^ old_state) >> 27u);
	const auto rot = static_cast<core::uint32>(old_state >> 59u);
	
	return (xor_shifted >> rot) | (xor_shifted << ((~rot + 1u) & 31));
}

rainbow::core::real rainbow::cpus::shared::pcg32::real(core::real min, core::real max)
{
	return uniform_real() * (max - min) + min;
}

rainbow::core::uint32 rainbow::cpus::shared::pcg32::uint32(core::uint32 min, core::uint32 max)
{
	return uint32(max - min + 1) + min;
}

rainbow::core::uint32 rainbow::cpus::shared::pcg32::uint32(core::uint32 value)
{
	const auto threshold = (~value + 1u) % value;
	while (true) {
		const auto r = uniform_uint32();
		if (r >= threshold) return r % value;
	}
}
