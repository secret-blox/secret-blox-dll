#include <windows.h>
#include <winternl.h>

#include "Internal/exchandler.hpp"
#include "Internal/memory.hpp"
#include "Internal/logger.hpp"

uintptr_t SB::Memory::base = 0;
HMODULE SB::Memory::hModule = 0;

typedef struct LDR_MODULE {
    LIST_ENTRY InLoadOrderModuleList;
    LIST_ENTRY InMemoryOrderModuleList;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID BaseAddress;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    SHORT LoadCount;
    SHORT TlsIndex;
    LIST_ENTRY HashTableEntry;
    ULONG TimeDateStamp;
} LDR_MODULE, * PLDR_MODULE;

#define UNLINK(x)					\
	(x).Flink->Blink = (x).Blink;	\
	(x).Blink->Flink = (x).Flink;

inline PLDR_MODULE getLoadOrderModuleList()
{
#ifdef _WIN64
    uintptr_t pPEB = (uintptr_t)__readgsqword(0x60);
    constexpr uint8_t loaderDataOff = 0x18;
    constexpr uint8_t inLoadOrderModuleList = 0x10;
#else
    uintptr_t pPEB = (uintptr_t)__readfsdword(0x30);
    constexpr uint8_t LoaderDataOff = 0xC;
    constexpr uint8_t InLoadOrderModuleList = 0xC;
#endif

    /* PEB->LoaderData->InLoadOrderModuleList */
    return (PLDR_MODULE)(*(uintptr_t*)(pPEB + loaderDataOff) + inLoadOrderModuleList);
}

void SB::Memory::unlinkModuleFromPEB()
{
    PLDR_MODULE StartEntry = getLoadOrderModuleList();
    PLDR_MODULE CurrentEntry = (PLDR_MODULE)StartEntry->InLoadOrderModuleList.Flink;

    while (StartEntry != CurrentEntry && CurrentEntry != NULL)
    {
        if (CurrentEntry->BaseAddress == hModule)
        {
            UNLINK(CurrentEntry->InLoadOrderModuleList);
            UNLINK(CurrentEntry->InMemoryOrderModuleList);
            UNLINK(CurrentEntry->InInitializationOrderModuleList);
            UNLINK(CurrentEntry->HashTableEntry);

            break;
        }

        CurrentEntry = (PLDR_MODULE)CurrentEntry->InLoadOrderModuleList.Flink;
    }
}

LPTOP_LEVEL_EXCEPTION_FILTER SB::Memory::originalFilter = nullptr;
void SB::Memory::setExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER filter)
{
    auto oldFilter = SetUnhandledExceptionFilter(filter);
    if (oldFilter && !originalFilter)
    {
        SB::Logger::printf("Memory: Original exception filter found at 0x%p\n", oldFilter);
		originalFilter = oldFilter;
    }
}

void SB::Memory::setup(HMODULE hModule)
{
    SB::Memory::hModule = hModule;

    // security
    // uniqua: I don't see the point to unlink the module from the PEB, but still good resource
	// unlinkModuleFromPEB(); // dll unload will crash as the dll will act to be already unloaded
    
    // TODO: register an instrumentation callback to handle exceptions dispatched to the process and log them and close the process before the game logs them
    // for now we alter the exception filter
    setExceptionFilter(defaultExceptionFilter);

	base = (uintptr_t)GetModuleHandleA(0);
}

void SB::Memory::unload()
{
    if (originalFilter)
    {
        setExceptionFilter(originalFilter);
        SB::Logger::printf("Memory: Exception filter restored\n");
    }
}
