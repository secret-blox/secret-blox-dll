#pragma once
#include "member.h"
#include <cstdint>


namespace module::security::obfuscation::number {
	struct obfuscatedNumber {
		static constexpr uint32_t serverKey = 0x18391e42;
		static constexpr uint32_t clientKey = 0xFE78912A;

		static inline uint32_t runtimeSKey = 0;
		static inline uint32_t runtimeCKey = 0;

		static __forceinline auto setKey(uint32_t key) -> void {
			runtimeCKey = clientKey;
			runtimeSKey = key;
		}

		uint32_t stored;
		auto __forceinline decode() -> uint32_t {
			return stored ^ runtimeSKey ^ runtimeCKey;
		}

		constexpr obfuscatedNumber(uint32_t target) : stored((uint32_t)target^ serverKey ^ clientKey) {}
	};

	struct obfuscatedNumberUncached {
		static constexpr uint32_t serverKey = 0x10787562;
		static constexpr uint32_t clientKey = 0xFE78912A;

		static inline uint32_t runtimeSKey = 0;
		static inline uint32_t runtimeCKey = 0;

		static auto __forceinline setKey(uint32_t key) -> void {
			runtimeCKey = clientKey;
			runtimeSKey = key;
		}

		uint32_t stored;
		auto __forceinline decode() -> uint32_t {
			while (runtimeSKey == 0)
				_mm_pause();

			return stored ^ runtimeSKey ^ runtimeCKey;
		}

		constexpr obfuscatedNumberUncached(uint32_t target) : stored((uint32_t)target^ serverKey ^ clientKey) {}
	};
}

#define obfuscate_number(n) (module::security::obfuscation::number::obfuscatedNumber(n).decode())
#define obfuscate_number_uncache(n) (module::security::obfuscation::number::obfuscatedNumberUncached(n).decode())