#include "framework.h"

#include "sb_utils/mem.h"
#include "sb_utils/rbx/api.h"

/// <summary>
/// entry.
/// </summary>
DWORD WINAPI startMain(LPVOID lpReserved) {
    // perform setups in order of priority
    SB::Memory::setup();
    SB::RBX::setup();

    SB::RBX::printf(SB::RBX::MESSAGE_INFO, "test");

    return TRUE;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Disables Direct Notification Calls For DLL_THREAD_ATTACH and DLL_THREAD_DETACH :spooky:
            DisableThreadLibraryCalls(hModule);
            
            // Not to hard to understand what this does, creates a thread blah blah blah hacker shit
            if (auto mainThread = CreateThread(
                nullptr, 
                0, 
                startMain, 
                hModule, 
                0, 
                nullptr
            ))
            {
                CloseHandle(hModule); // Closes the handle if the thread returns a nullptr / or 0 in dottik terms
            }
        }
    }
    return TRUE;
}

// Return call back for our hook, "secretBlox", yes exporting will this but not to worry as we are white listed so we don't care if we show 
extern "C" __declspec(dllexport) int secretBlox(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}
