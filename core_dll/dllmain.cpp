#include "offsets/functions.h"
#include <Windows.h>

// Call Point to our starter thread, this will run once our thread has been created and point to this function :heheh:

DWORD WINAPI startMain(LPVOID lpReserved) {
    // r_print (0: Normal Print, 1: Information, 2: Warning, 3: Error)
    rbxPrint("Error", "Now i just cant bring myself away");

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
            auto mainThread = CreateThread(
                nullptr, 
                0, 
                startMain, 
                hModule, 
                0, 
                nullptr
            );
            
            // Closes the handle if the thread returns a nullptr / or 0 in dottik terms

           
            if (mainThread) {
                CloseHandle(hModule);
            }

        }
    }
    return TRUE;
}

// Return call back for our hook, "secretBlox", yes exporting will this but not to worry as we are white listed so we don't care if we show 
extern "C" __declspec(dllexport) int secretBlox(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}
