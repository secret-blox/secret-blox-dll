#include "Internal/exchandler.hpp"

// good resource: https://github.com/tapika/stacktrace/blob/develop/src/exception_handler.cpp

LONG __stdcall SB::Memory::defaultExceptionFilter(EXCEPTION_POINTERS* exceptionInfo)
{

	return 0;
}