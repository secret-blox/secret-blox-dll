#include "framework.hpp"

#include "Internal/utils.hpp"
#include "Internal/logger.hpp"
#include "Internal/memory.hpp"
#include "Internal/execution.hpp"

#include "Rbx/api.hpp"
#include "Rbx/taskscheduler.hpp"
#include "Rbx/scriptcontext.hpp"

#include "Security/callstackspoof.h"
#include "Security/xor.hpp"

#include "lstate.h"
#include "offsets.hpp"

HMODULE dllModule = 0;

void debug()
{
    SPOOF_FUNC;
    SB::Logger::printf(xorstr_("Internal Base: %p\n"), SB::Memory::base);

    const auto taskScheduler = SB::Rbx::TaskScheduler::get();
    SB::Logger::printf(xorstr_("TaskScheduler: %p\n"), taskScheduler.baseAddress);

    const auto jobs = taskScheduler.getJobs();
    SB::Logger::printf(xorstr_("Jobs: %d\n"), jobs.size());

    const auto luaGc = taskScheduler.getJobByName("LuaGc");
    if (luaGc.has_value())
    {
        const auto scriptContextInst = luaGc->getScriptContext();
        const auto dataModel = scriptContextInst.getParent();

        SB::Logger::printf(xorstr_("LuaGc: %p\n"), luaGc->baseAddress);
        SB::Logger::printf(xorstr_("DataModel: %p\n"), dataModel.baseAddress);
        SB::Logger::printf(xorstr_("ScriptContext: %p\n"), scriptContextInst.baseAddress);

        const auto children = scriptContextInst.getChildren();
        SB::Logger::printf(xorstr_("ScriptContext Children: %d\n"), children.size());

        const SB::Rbx::ScriptContext scriptContext = { scriptContextInst.baseAddress };
        //scriptContext.debugGetLuaState();
        const auto L = scriptContext.getLuaState();
        if (!L)
            return;
        SB::Logger::printf(xorstr_("lua_State: %p\n"), L);
        // DEBUG PURPOSE CODE
        // try printing a simple tvalue number
        /*
        lua_pushnumber(luaState, 123);
        using luau_warnCC = int64_t __fastcall(lua_State*);
        auto luau_warn = reinterpret_cast<luau_warnCC*>(SB::Memory::base + 0xF2A1A0);
        luau_warn(luaState);
        */
        /*
        SB::Logger::printf(
            xorstr_("original deobf globalState %p, macro deobf globalState %p\n"),
            (uintptr_t)L + *(uintptr_t *)((uintptr_t)L + 32),
            (uintptr_t)(global_State*)L->global
        );
        */
    }
    else
        SB::Logger::printf(xorstr_("LuaGc not found\n"));
}

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
    // SB::Websocket::setup();
    // perform offsets version check before running important setups
    if (!checkOffsetsVersion())
    {
        // TODO: communicate with ui
        SB::Logger::printf(xorstr_("Invalid offsets version, update offsets!\n"));
        return FALSE;
    }
    #ifdef DEBUG_MODE
    debug();
    #endif

    // TODO: perform important setups in order of priority
    // SB::Execution::setup();
    SB::Logger::printf(xorstr_("Internal: Loaded\n"));

    // TODO: check if websocket setupped correctly
    // TODO: check if execution setupped correctly
    
    
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
