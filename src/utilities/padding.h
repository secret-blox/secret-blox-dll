#pragma once

#ifndef PADDING_HPP
#define PADDING_HPP

#include <cstdint>
#include <type_traits>

#ifndef CONCAT
#	define _CONCAT_IMPL(a, b) a ## b
#	define CONCAT(a, b) _CONCAT_IMPL(a, b)
#endif

template<std::ptrdiff_t baseOffset, std::ptrdiff_t offset>
constexpr std::ptrdiff_t pValue = offset - baseOffset;

template<std::ptrdiff_t padValue>
struct padding {
	static_assert(padValue >= 0, "cannot declare a member at ab offset lower than the last member offset of a base structure inside of a derived structure");
	std::uint8_t pad[padValue];
};

template <>
struct padding<0>{};

#define PAD_FIELD(T, name, off) struct : padding<pValue<_base_offset, off>> { T name; };
#define _PAD_START(size) static constexpr auto _base_offset = std::integral_constant<std::ptrdiff_t, size>{}; union
#define PAD_START_DERIVED(baseClass) _PAD_START(sizeof(baseClass))
#define PAD_START() _PAD_START(0)

#endif