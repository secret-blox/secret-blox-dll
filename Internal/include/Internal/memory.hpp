#pragma once

#include <windows.h>
#include <stdint.h>

#define SB_OFFSET(x) (SB::Memory::base + x)

namespace SB::Memory {
	extern uintptr_t base;
	extern HMODULE hModule;

	void unlinkModuleFromPEB();

	/// <summary>
	/// SB::Memory setup function, responsible for initializing memory utils.
	/// </summary>
	void setup(HMODULE hModule);
}