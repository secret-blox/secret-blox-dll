#pragma once
#include <cstdint>

template<typename T, uint32_t capacity>
static constexpr auto fill(T(*initializer)()) {
	std::array<T, capacity> target{};

	for (T& ref : target)
		ref = initializer();

	return target;
}