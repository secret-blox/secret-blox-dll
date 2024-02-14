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
            DisableThreadLibraryCalls(hModule);
            
            if (auto mainThread = CreateThread(
                nullptr, 
                0, 
                startMain, 
                hModule, 
                0, 
                nullptr
            ))
            {
                CloseHandle(hModule);
            }
        }
    }
    return TRUE;
}

extern "C" __declspec(dllexport) int secretBlox(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}
