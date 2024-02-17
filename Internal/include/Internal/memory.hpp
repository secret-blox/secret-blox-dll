#pragma once
#include <stdint.h>

#define SB_OFFSET(x) (SB::Memory::base + x)

namespace SB::Memory {
	extern uintptr_t base;

	/// <summary>
	/// SB::Memory setup function, responsible for initializing memory utils.
	/// </summary>
	void setup();
}