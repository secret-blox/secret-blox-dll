#include "Internal/test.hpp"
#include "Rbx/scriptcontext.hpp"
#include "Rbx/taskscheduler.hpp"

#include "Internal/execution.hpp"
#include "Internal/logger.hpp"
#include "Internal/memory.hpp"
#include "Security/xor.hpp"

#include "lua.h"
#include "lapi.h"
#include "lstate.h"

#define SB_ASSERT(expr) if (!(expr)) { SB::Logger::printf("Assertion failed, %s, at line %d. \n", #expr, __LINE__); return false; }

using luaCFunCC = int64_t __fastcall(lua_State*);

bool SB::Test::run()
{
    SB::Logger::printf(XORSTR("Internal Base: %p\n"), SB::Memory::base);

    // exception handler will caugh this in case of failure
    const auto taskScheduler = SB::Rbx::TaskScheduler::get();
    SB::Logger::printf(XORSTR("TaskScheduler: %p\n"), taskScheduler.baseAddress);

    const auto jobs = taskScheduler.getJobs();
    const auto jobsSize = jobs.size();
    SB_ASSERT(jobsSize >= 20 && jobsSize <= 80); // jobs size should be between 20 and 80
    SB::Logger::printf(XORSTR("Jobs: %d\n"), jobs.size());

    const auto luaGc = taskScheduler.getJobByName("LuaGc");
    SB_ASSERT(luaGc.has_value()); // LuaGc job should exist

    Rbx::ScriptContext scriptContext = luaGc->getScriptContext();
    SB_ASSERT(scriptContext.getBaseAddress() != 0);
    Rbx::Instance dataModel = scriptContext.getParent();
    SB_ASSERT(dataModel.getBaseAddress() != 0);
    SB_ASSERT(scriptContext.getClassName() == "ScriptContext");

    lua_State* RL = scriptContext.getLuaState();
    SB_ASSERT(RL != nullptr);
    SB::Logger::printf(XORSTR("lua_State: %p\n"), RL);

    auto gt = RL->gt;
    SB::Logger::printf(XORSTR("GlobalTable: %p\n"), gt);
    SB_ASSERT(gt != nullptr);
    SB_ASSERT(gt->tt == LUA_TTABLE);
    
    // verify state & tvalue integrity by grabbing globaltable
    const auto gt2 = luaA_toobject(RL, LUA_GLOBALSINDEX);
    SB_ASSERT((uintptr_t)gt2->value.gc == (uintptr_t)gt);
    SB::Logger::printf(XORSTR("Start Top: %d\n"), lua_gettop(RL));

    /*
    lua_pushstring(RL, "epicstring");
    auto warnCC = reinterpret_cast<luaCFunCC*>SB_OFFSET(0xf2a1a0);
    warnCC(RL);
    */

    // verify table struct & tstring
    lua_rawgetfield(RL, LUA_GLOBALSINDEX, "_VERSION");
    auto version = *luaA_toobject(RL, -1);
    lua_pop(RL, 1);
    SB_ASSERT(version.tt == LUA_TSTRING);
    // version->value.gc->ts.data;
    SB_ASSERT(strcmp(version.value.gc->ts.data, "Luau") == 0);

    // verify closure struct
    auto taskTT = lua_rawgetfield(RL, LUA_GLOBALSINDEX, "task");
    SB_ASSERT(taskTT == LUA_TTABLE);
    lua_rawgetfield(RL, -1, "spawn");
    auto spawn = *luaA_toobject(RL, -1);
    lua_pop(RL, 2);
    SB_ASSERT(spawn.tt == LUA_TFUNCTION);
    auto& spawnClosure = spawn.value.gc->cl;
    SB_ASSERT(spawnClosure.isC == 1);
    SB_ASSERT(spawnClosure.nupvalues == 0);
    SB_ASSERT(strcmp((const char*)spawnClosure.c.debugname, "spawn") == 0);
    SB::Logger::printf(
        XORSTR("spawnClosure offset: %p\n"), 
        (uintptr_t)((lua_CFunction)spawnClosure.c.f) - SB::Memory::base
    );
    
    // TODO: fix crash caused by some misconfiguration vmvalues/shuffles
    RbxBytecodeEncoder encoder;
    auto code = XORSTR("print(1 + 1);");
    std::string Bytecode = Luau::compile(code, {}, {}, &encoder);
    luau_load(RL, "=game", Bytecode.c_str(), Bytecode.size(), 0);
    lua_CFunction spawnF = spawnClosure.c.f;
    spawnF(RL);
    lua_pop(RL, 1);

    SB::Logger::printf(XORSTR("End Top: %d\n"), lua_gettop(RL));
    return true;
}