#pragma once
#include <array>
#include <utility>

#include "random.h"
#include <utilities/fill.h>
#include <utilities/utils/utils.h>

namespace module::security::obfuscation::member {
	template<typename T, uint32_t counter>
	struct obfuscatedMember {
		T value;

		static constexpr uint32_t runtimeCount = 64;

		using byteArray = std::array<uint8_t, sizeof(T)>;
		using byteArrayArray = std::array<byteArray, runtimeCount>;

		static constexpr byteArrayArray runtime = fill<byteArray, runtimeCount>([]() { return uniformDistribution<uint8_t, sizeof(T), counter>(0, 255); });

		auto encode(T& ref) -> void {
			constexpr byteArray target = runtime[counter % runtimeCount];
			for (unsigned i = 0; i < sizeof(T); i++)
				((uint8_t*)&ref)[i] ^= target[i];
		}

		auto decode() -> T {
			constexpr byteArray target = runtime[counter % runtimeCount];
			T res = std::move(this->value);
			for (unsigned i = 0; i < sizeof(T); i++)
				((uint8_t*)&res)[i] ^= target[i];

			return res;
		}

		obfuscatedMember() {}

		__forceinline obfuscatedMember(T ref) : value(std::move(ref)) {
			encode(this->value);
		}

		__forceinline auto operator*() -> T {
			return decode();
		}
	};

	class obfuscatedMemberString {
	private:
		uint32_t xorKey;
		std::string value;

	public:
		explicit obfuscatedMemberString(std::string& Val) : value(std::move(Val)) {
			utils* utils = utils::getSingleton();
			xorKey = utils->randomInteger(1, UINT_MAX);
		}

		auto __forceinline process() -> void {
			for (size_t i = 0; i < value.size(); i++)
			{
				value[i] ^= ((uint8_t*)&xorKey)[i % 3] % 200;
			}
		}

		auto get() -> std::string {
			return std::string(value.begin(), value.end());
		}
	};
}

#define obfuscate_member(t) module::security::obfuscation::member::obfuscatedMember<t, __COUNTER__>