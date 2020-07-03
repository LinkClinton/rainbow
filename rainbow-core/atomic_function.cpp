#include "atomic_function.hpp"

void rainbow::core::atomic_real_add(std::atomic<real>& value, real add)
{
	auto old_value = value.load();
	auto new_value = old_value + add;

	while (!value.compare_exchange_weak(old_value, new_value));
}
