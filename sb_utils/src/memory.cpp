#include "mem.h"
#include "framework.h"

extern uintptr_t SB::Memory::base = 0;

void SB::Memory::setup()
{
	base = (uintptr_t)GetModuleHandleA(0);
}
