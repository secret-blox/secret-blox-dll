#include "framework.h"

#include "sb_utils/utils.h"
#include "sb_utils/mem.h"
#include "sb_utils/rbx/api.h"
#include "sb_utils/rbx/taskscheduler.h"
#include "sb_utils/offsets.h"

HMODULE dllModule = 0;

/// <summary>
/// entry.
/// </summary>
DWORD WINAPI startMain(LPVOID lpReserved) {
    // perform setups in order of priority
    SB::Memory::setup();
    SB::RBX::setup();

    // setup log file
    auto dllDir = SB::Utils::getDllDir(dllModule);
    std::ofstream log(dllDir / "log.txt");

    log << "base: " << std::hex << SB::Memory::base << std::endl;

    const auto taskScheduler = SB::RBX::TaskScheduler::get();

    log << "taskScheduler: " << std::hex << taskScheduler.baseAddress << std::endl;


    const auto jobAddresses = taskScheduler.getAllJobs();

    log << "Registered jobs: " << std::dec << jobAddresses.size() << std::endl;

    for (const auto& jobAddress : jobAddresses) {
        const char* jobName = taskScheduler.getJobName(jobAddress);
        log << "Job: 0x" << std::hex << jobAddress << ", Name: " << jobName << std::endl;
    }

    const uintptr_t LuaGc = taskScheduler.getJobByName("LuaGc");

    log << "Found LuaGc: 0x" << std::hex << LuaGc << std::endl;

    log.close();

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
