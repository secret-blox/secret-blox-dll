#include <map>

#include "Internal/logger.hpp"
#include "Internal/exchandler.hpp"

#include "Security/xor.hpp"

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
    unsigned int code = exceptionInfo->ExceptionRecord->ExceptionCode;
    const char* str = XORSTR("Unknown exception code");
    if (exceptionCodes.find(code) != exceptionCodes.end())
        str = exceptionCodes.at(code);

    SB::Logger::printf(
        XORSTR("CRASH DETECTED"
        "\n\nException caught: %s\n" \
        "Exception code: 0x%x\n" \
        "Exception address: 0x%p\n" \
        "\nContext:\n" \
        "RIP: 0x%p\n" \
        "RSP: 0x%p\n"
        "RBP: 0x%p\n"
        "RDX: 0x%p\n"
        "RCX: 0x%p\n"
        "RBX: 0x%p\n"
        "RDI: 0x%p\n"
        "RSI: 0x%p\n"
        "R8: 0x%p\n"
        "R9: 0x%p\n"
        "R10: 0x%p\n"
        "R11: 0x%p\n"
        "R12: 0x%p\n"
        "R13: 0x%p\n"
        "R14: 0x%p\n"
        "R15: 0x%p\n"
        ),
        // head
        str,
        code,
        exceptionInfo->ExceptionRecord->ExceptionAddress,
        // stack trace
        exceptionInfo->ContextRecord->Rip,
        exceptionInfo->ContextRecord->Rsp,
        exceptionInfo->ContextRecord->Rbp,
        exceptionInfo->ContextRecord->Rdx,
        exceptionInfo->ContextRecord->Rcx,
        exceptionInfo->ContextRecord->Rbx,
        exceptionInfo->ContextRecord->Rdi,
        exceptionInfo->ContextRecord->Rsi,
        exceptionInfo->ContextRecord->R8,
        exceptionInfo->ContextRecord->R9,
        exceptionInfo->ContextRecord->R10,
        exceptionInfo->ContextRecord->R11,
        exceptionInfo->ContextRecord->R12,
        exceptionInfo->ContextRecord->R13,
        exceptionInfo->ContextRecord->R14,
        exceptionInfo->ContextRecord->R15
    );

    ExitProcess(0);

    if (originalFilter)
	    return originalFilter(exceptionInfo);

	return EXCEPTION_EXECUTE_HANDLER;
}