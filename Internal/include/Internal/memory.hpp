#pragma once
#pragma comment(lib, "Kernel32.lib")

#include <windows.h>
#include <stdint.h>

extern "C" WINBASEAPI
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI SetUnhandledExceptionFilter(
	_In_opt_ LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
);

#define SB_OFFSET(x) (SB::Memory::base + x)

namespace SB::Memory {
	extern uintptr_t base;
	extern HMODULE hModule;

	void unlinkModuleFromPEB();

	extern LPTOP_LEVEL_EXCEPTION_FILTER originalFilter;
	void setExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER filter);

	/// <summary>
	/// SB::Memory setup function, responsible for initializing memory utils.
	/// </summary>
	void setup(HMODULE hModule);
	/// <summary>
	/// SB::Memory unload function, responsible for unloading memory utils.
	/// </summary>
	void unload();
}