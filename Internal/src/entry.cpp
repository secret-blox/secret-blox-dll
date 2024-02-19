#include "framework.hpp"

#include "Internal/utils.hpp"
#include "Internal/logger.hpp"
#include "Internal/memory.hpp"

#include "Rbx/api.hpp"
#include "Rbx/taskscheduler.hpp"

#include "offsets.hpp"

HMODULE dllModule = 0;

void debug()
{
    SB::Logger::printf("Internal Base: %p\n", SB::Memory::base);

    const auto taskScheduler = SB::Rbx::TaskScheduler::get();
    SB::Logger::printf("TaskScheduler: %p\n", taskScheduler.baseAddress);

    const auto jobs = taskScheduler.getJobs();
    SB::Logger::printf("Jobs: %d\n", jobs.size());

    const auto luaGc = taskScheduler.getJobByName("LuaGc");
    if (luaGc.has_value())
    {
        const auto scriptContext = luaGc->getScriptContext();
        const auto dataModel = scriptContext.getParent();

        SB::Logger::printf("LuaGc: %p\n", luaGc->baseAddress);
        SB::Logger::printf("DataModel: %p\n", dataModel.baseAddress);
        SB::Logger::printf("ScriptContext: %p\n", scriptContext.baseAddress);

        const auto children = scriptContext.getChildren();
        SB::Logger::printf("ScriptContext Children: %d\n", children.size());
    }
}

bool checkOffsetsVersion()
{
    const auto rbxModule = reinterpret_cast<HMODULE>(SB::Memory::base);
    if (SB::Utils::getDllDir(rbxModule).filename() != SB::Offsets::Rbx::version)
    {
		SB::Logger::printf("Invalid Roblox module\n");
		return false;
	}
    return true;
}

DWORD WINAPI startMain(LPVOID lpReserved) {
    // perform core setups in order of priority
    SB::Logger::setup(SB::Utils::getDllDir(dllModule));
    SB::Memory::setup(dllModule);
    SB::Rbx::setup();
    // SB::Websocket::setup();
    // perform offsets version check before running important setups
    if (!checkOffsetsVersion())
    {
        // TODO: communicate with ui
        SB::Logger::printf("Invalid offsets version, update offsets!\n");
        return FALSE;
    }
    // TODO: perform important setups in order of priority
    // SB::Execution::setup();
    SB::Logger::printf("Internal: Loaded\n"); 

    // TODO: check if websocket setupped correctly
    // TODO: check if execution setupped correctly

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
        break;
    }
    case DLL_PROCESS_DETACH:
		SB::Memory::unload();
        SB::Logger::unload(); // unload logger for last
		break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) int secretBlox(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}
