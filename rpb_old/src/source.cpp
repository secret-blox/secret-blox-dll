#include <windows.h>
#include <thread>

#include <lualib.h>
#include <dependencies/luau/vm/src/lstate.h>
#include <roblox/exploit/scheduler/scheduler.h>
#include <roblox/exploit/execution/execution.h>
#include <Luau/Compiler.h>


const auto execution{ module::execution::getSingleton() };

//auto loadstring(lua_State* L) -> int {
//    std::string code{ lua_tostring(L, 1) };
//    const std::string& bytecode = Luau::compile(code);
//
//    if (luau_load(L, obfuscate_str("ZAMNZADSDDYYY"), bytecode.c_str(), bytecode.size(), 0) != LUA_OK) {
//        std::string err = lua_tostring(L, -1);
//        lua_pop(L, 1);
//
//        lua_pushnil(L);
//        lua_pushlstring(L, err.c_str(), err.size());
//        return 2;
//    }
//
//    return 1;
//}

auto mainThread() -> void {
    auto taskScheduler{ module::scheduler::getSingleton() };

    /*lua_State* L = taskScheduler->getLuaState();
    lua_pushcclosurek(L, loadstring, nullptr, 0, nullptr);
    lua_setglobal(L, "loadstring");*/

    execution->load();
    /*

    lua_getglobal(L, obfuscate_str("loadstring"));
    Closure* cl = (Closure*)lua_topointer(L, -1);
    uintptr_t address = unbase(reinterpret_cast<uintptr_t>(cl->c.f));
    address += 0x26681270000;*/

    //module::roblox::stdOut(0, obfuscate_str("loadstring addy: 0x%016llx"), address);

    std::string code{ obfuscate_str("print({});print(\"Hello From Inside the main x64 client!\");\nwarn(\"ZAMNNNNNNNN\")") };
    execution->send(code);
    //execution->send("while task.wait(.5) do printidentity(\"Current Cuh Identity Has To Be\");end"); //  warn(game:GetService(\"CoreGui\")) 



    //execution->createPipe();

    return;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD callReason, LPVOID lpReserved) {
    if (callReason == DLL_PROCESS_ATTACH) {
        std::thread(mainThread).detach();
    }
    
    return TRUE;
}

