#pragma once
#include <cstdint>
class Addresses {
	public:
		// Define macro of (ptr + ModuleBase)
		#define moduleBase(ptr) (ptr + (uintptr_t)GetModuleHandleA(0))
		static uintptr_t print_Address;

};