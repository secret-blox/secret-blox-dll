#include <map>
#include <windows.h>
#include <dbghelp.h>

#include "Internal/logger.hpp"
#include "Internal/exchandler.hpp"

#include "Security/xor.hpp"

#pragma comment(lib, "dbghelp.lib")

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

#define MAX_STACK_FRAMES 10
void printStacks(CONTEXT context)
{
    BOOL                result;
    HANDLE              process;
    HANDLE              thread;
    STACKFRAME64        stack;
    ULONG               frame;
    DWORD64             displacement;

    process                = GetCurrentProcess();
    thread                 = GetCurrentThread();
    displacement           = 0;

    // Initialize stack
    memset( &stack, 0, sizeof( STACKFRAME64 ) );
    stack.AddrPC.Offset    = context.Rip;
    stack.AddrPC.Mode      = AddrModeFlat;
    stack.AddrStack.Offset = context.Rsp;
    stack.AddrStack.Mode   = AddrModeFlat;
    stack.AddrFrame.Offset = context.Rbp;
    stack.AddrFrame.Mode   = AddrModeFlat;

    
    for( frame = 0; frame < MAX_STACK_FRAMES; frame++ )
    {
        result = StackWalk64
        (
            IMAGE_FILE_MACHINE_AMD64,
            process,
            thread,
            &stack,
            &context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );
        if (!result)
            break;

        // If the return address is zero, we're done
        if (stack.AddrReturn.Offset == 0) {
            break;
        }

        SB::Logger::printf
        (
            XORSTR("\n\nFrame %lu:\n"
            "    Symbol name:    ???\n"
            "    PC address:     0x%08LX\n"
            "    Stack address:  0x%08LX\n"
            "    Frame address:  0x%08LX\n"
            "\n"),
            frame,
            ( ULONG64 )stack.AddrPC.Offset,
            ( ULONG64 )stack.AddrStack.Offset,
            ( ULONG64 )stack.AddrFrame.Offset
        );
    }
}

LONG __stdcall SB::Memory::defaultExceptionFilter(EXCEPTION_POINTERS* exceptionInfo)
{
    // TODO: freeze all threads

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
        // context
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

    printStacks(*exceptionInfo->ContextRecord);

    ExitProcess(0);

    if (originalFilter)
	    return originalFilter(exceptionInfo);

	return EXCEPTION_EXECUTE_HANDLER;
}