#pragma once

#include <Windows.h>

namespace SB::Memory
{
	extern LPTOP_LEVEL_EXCEPTION_FILTER originalFilter;
	LONG __stdcall defaultExceptionFilter(EXCEPTION_POINTERS* exceptionInfo);
}