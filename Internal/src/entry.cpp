#include "framework.hpp"

#include "Internal/utils.hpp"
#include "Internal/logger.hpp"
#include "Internal/memory.hpp"
#include "Internal/execution.hpp"
#include "Internal/test.hpp"
#include "Internal/scheduler.hpp"
#include "Internal/websocket.hpp"

#include "Rbx/rapi.hpp"

#include "Security/callstackspoof.h"
#include "Security/xor.hpp"

#include "lstate.h"
#include "offsets.hpp"

HMODULE dllModule = 0;
bool checkOffsetsVersion()
{
    SPOOF_FUNC;
    const auto rbxModule = reinterpret_cast<HMODULE>(SB::Memory::base);
    if (SB::Utils::getDllDir(rbxModule).filename() != SB::Offsets::Rbx::version)
		return false;
    return true;
}

DWORD WINAPI startMain(LPVOID lpReserved) {
    SPOOF_FUNC;
    // perform core setups in order of priority
    SB::Logger::setup(SB::Utils::getDllDir(dllModule));
    SB::Memory::setup(dllModule);
    SB::Rbx::setup();
    SB::Logger::printf(XORSTR("Internal Base: %p\n"), SB::Memory::base);
    SB::Websocket::setup();
    
    // perform offsets version check before running important setups
    if (!checkOffsetsVersion())
    {
        // TODO: communicate with ui
        SB::Logger::printf(XORSTR("Invalid offsets version, update offsets!\n"));
        return FALSE;
    }
    
    #ifdef UNIT_TEST
    SB::Logger::printf(XORSTR("UNIT TEST\n"));
    if(!SB::Test::run())
    {
        SB::Logger::printf(XORSTR("Unit test failed\n"));
        return FALSE;
    }
    SB::Logger::printf(XORSTR("UNIT TEST PASSED\n"));
    #endif

    // perform important setups in order of priority
    SB::Scheduler::setup();
    SB::Execution::setup();
    SB::Logger::printf(XORSTR("Internal: Loaded\n"));
    
    return TRUE;
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)   
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        
        DisableThreadLibraryCalls(hModule);

        dllModule = hModule;

        if (auto mainThread = CreateThread(
            nullptr,
            0,
            startMain,
            nullptr, // no need to pass HMODULE as parameter to CreateThread
            0,
            nullptr
        ))
        {
            
            CloseHandle(hModule);
        }
        break;
    }
    case DLL_PROCESS_DETACH:
        SB::Scheduler::unload();
		SB::Memory::unload();
        SB::Websocket::unload();
        SB::Logger::unload(); // unload logger for last
		break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) int secretBlox(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}
