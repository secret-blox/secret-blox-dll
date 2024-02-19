#include <map>

#include "Internal/logger.hpp"
#include "Internal/exchandler.hpp"

#include "callstackspoof.h"
#include "xor.hpp"

// resource: https://github.com/tapika/stacktrace/blob/develop/src/exception_handler.cpp
#define STACKTRACE_EXCEPTION_STRING(Code) { Code, #Code },
const std::map<unsigned int, const char*> exceptionCodes =
{
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_ACCESS_VIOLATION)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_BREAKPOINT)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_SINGLE_STEP)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_ARRAY_BOUNDS_EXCEEDED)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_FLT_DENORMAL_OPERAND)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_FLT_DIVIDE_BY_ZERO)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_FLT_INEXACT_RESULT)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_FLT_INVALID_OPERATION)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_FLT_OVERFLOW)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_FLT_STACK_CHECK)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_FLT_UNDERFLOW)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_INT_DIVIDE_BY_ZERO)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_INT_OVERFLOW)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_PRIV_INSTRUCTION)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_IN_PAGE_ERROR)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_ILLEGAL_INSTRUCTION)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_NONCONTINUABLE_EXCEPTION)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_STACK_OVERFLOW)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_INVALID_DISPOSITION)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_GUARD_PAGE)
        STACKTRACE_EXCEPTION_STRING(EXCEPTION_INVALID_HANDLE)
};

LONG __stdcall SB::Memory::defaultExceptionFilter(EXCEPTION_POINTERS* exceptionInfo)
{
    SPOOF_FUNC;
    unsigned int code = exceptionInfo->ExceptionRecord->ExceptionCode;
    const char* str = xorstr_("Unknown exception code");
    if (exceptionCodes.find(code) != exceptionCodes.end())
        str = exceptionCodes.at(code);
    
    // TODO: Added Xor
    SB::Logger::printf(
        "CRASH DETECTED"
        "\n\nException caught: %s\n" \
        "Exception code: 0x%x\n" \
        "Exception address: 0x%p\n" \
        "\nStack trace:\n" \
        "RIP: 0x%p\n" \
        "RSP: 0x%p\n",
        // head
        str,
        code,
        exceptionInfo->ExceptionRecord->ExceptionAddress,
        // stack trace
        exceptionInfo->ContextRecord->Rip,
        exceptionInfo->ContextRecord->Rsp
    );

    ExitProcess(0);

    if (originalFilter)
	    return originalFilter(exceptionInfo);

	return EXCEPTION_EXECUTE_HANDLER;
}