#include "framework.hpp"

#include "Internal/utils.hpp"
#include "Internal/memory.hpp"

#include "Rbx/api.hpp"
#include "Rbx/taskscheduler.hpp"

#include "offsets.hpp"

HMODULE dllModule = 0;

void debug()
{
    // setup log file
    auto dllDir = SB::Utils::getDllDir(dllModule);
    std::ofstream log(dllDir / "log.txt");
    log << "Base: " << std::hex << SB::Memory::base << std::endl;

    const auto taskScheduler = SB::Rbx::TaskScheduler::get();
    log << "TaskScheduler: " << std::hex << taskScheduler.baseAddress << std::endl;

    const auto jobs = taskScheduler.getJobs();
    log << "Jobs: " << std::dec << jobs.size() << std::endl;

    const auto luaGc = taskScheduler.getJobByName("LuaGc");
    if (luaGc.has_value())
    {
        const auto scriptContext = luaGc->getScriptContext();
        const auto dataModel = scriptContext.getParent();

        log << "DataModel: " << std::hex << dataModel.baseAddress << std::endl;
        log << "ScriptContext: " << std::hex << scriptContext.baseAddress << std::endl;

        const auto children = scriptContext.getChildren();
        log << "ScriptContext Children: " << std::dec << children.size() << std::endl;
        for (const auto& service : children)
		{
			log << std::hex << service.baseAddress << ": " << service.getName() << std::endl;
		}
    }
}

DWORD WINAPI startMain(LPVOID lpReserved) {
    // perform setups in order of priority
    SB::Memory::setup(dllModule);
    SB::Rbx::setup();

    debug();
    
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
    }
    }
    return TRUE;
}

extern "C" __declspec(dllexport) int secretBlox(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}
