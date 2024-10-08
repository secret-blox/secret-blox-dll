#pragma once
#include <cstdint>
#include <string>
#include "random.hpp"
#pragma warning(disable: 4307)

namespace SB::Security
{
	template <uint32_t size, uint32_t counter>
	struct ObfuscatedString {
		static constexpr std::array<uint8_t, size> XOR = uniformDistribution<uint8_t, size, counter>(0, 255);
		std::array<char, size> encrypted;

		static constexpr __forceinline auto encrypt(uint32_t idx, char C) -> char {
			return C ^ XOR[idx];
		}

		template <size_t... idx>
		__forceinline constexpr ObfuscatedString(const char* str, std::index_sequence<idx...>) :
			encrypted({ encrypt(idx, str[idx])... }) { }

		__forceinline const char* decrypt() noexcept {
			for (uint32_t x = 0; x < size; x++)
				encrypted[x] ^= XOR[x];

			return encrypted.data();
		}

		// not needed as const char* to std::string is well handled
		__forceinline std::string decryptStr() noexcept {
			for (uint32_t x = 0; x < size; x++)
				encrypted[x] ^= XOR[x];

			return std::string{ encrypted.data() };
		}
	};
}

#define OBFSTR(s) (SB::Security::ObfuscatedString<sizeof(s), __COUNTER__>(s, std::make_index_sequence<sizeof(s)>()).decrypt())