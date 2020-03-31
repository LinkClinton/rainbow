#include "random_generator.hpp"

#include <ctime>

rainbow::random_generator::random_generator() : random_generator(static_cast<uint64>(time(nullptr)))
{
	
}

rainbow::random_generator::random_generator(const uint64 seed) : mEngine(seed), mDistribution(0, 1)
{
}

rainbow::real rainbow::random_generator::normalize_real() 
{
	return mDistribution(mEngine);
}

rainbow::real rainbow::random_generator::real(const rainbow::real min, const rainbow::real max) 
{
	const auto distribution = std::uniform_real_distribution<rainbow::real>(min, max);

	return distribution(mEngine);
}
