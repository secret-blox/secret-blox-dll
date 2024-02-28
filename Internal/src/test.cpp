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

#include "sb_scripts.hpp"

#define SB_ASSERT(expr) if (!(expr)) { SB::Logger::printf("Assertion failed, %s, at line %d. \n", #expr, __LINE__); return false; }

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

    // SETUP EXECUTION
    SB::Execution::rState = RL;

    /*
    lua_pushstring(RL, "epicstring");
    auto warnCC = reinterpret_cast<luaCFunCC*>SB_OFFSET(0xf2a1a0);
    warnCC(RL);
    */
   // TODO: verify TString before performing operations on tables

    // verify table struct & tstring
    lua_rawgetfield(RL, LUA_GLOBALSINDEX, "_VERSION");
    auto version = *luaA_toobject(RL, -1);
    lua_pop(RL, 1);
    SB_ASSERT(version.tt == LUA_TSTRING);
    // version->value.gc->ts.data;
    SB_ASSERT(strcmp(version.value.gc->ts.data, "Luau") == 0);

     // check table metatable by looking at game metatable
    lua_rawgetfield(RL, LUA_GLOBALSINDEX, "game");
    auto game = *luaA_toobject(RL, -1);
    SB_ASSERT(game.tt == LUA_TUSERDATA);
    SB_ASSERT(game.value.gc->u.metatable != nullptr);
    SB_ASSERT(game.value.gc->u.metatable->tt == LUA_TTABLE);
    lua_pop(RL, 1);

    // verify closure struct
    lua_rawgetfield(RL, LUA_GLOBALSINDEX, "warn");
    auto warnTV = *luaA_toobject(RL, -1);
    lua_pop(RL, 1);
    SB_ASSERT(warnTV.tt == LUA_TFUNCTION);
    auto& warnClosure = warnTV.value.gc->cl;
    SB_ASSERT(warnClosure.isC == 1);
    SB_ASSERT(warnClosure.nupvalues == 0);
    SB_ASSERT(strcmp((const char*)warnClosure.c.debugname, "warn") == 0);

    // SETUP EXECUTION 2
    SB::Execution::coCreate = SB::Execution::getLibraryFunc(RL, "coroutine", "create");
    SB_ASSERT(SB::Execution::coCreate != nullptr);
    SB::Execution::taskDefer = SB::Execution::getLibraryFunc(RL, "task", "defer");
    SB_ASSERT(SB::Execution::taskDefer != nullptr);
    SB::Logger::printf(XORSTR("coCreate: %p\n"), (uintptr_t)SB::Execution::coCreate - SB::Memory::base);
    // create eState
    auto thread = SB::Execution::createThread(RL);
    SB::Execution::eState = thread;
    lua_pop(thread, 1);
    SB::Execution::setIdentity(thread, SB::Execution::_8_Replicator);
    SB::Logger::printf(XORSTR("eState: %p\n"), (uintptr_t)thread);
    
    auto code = SB_SCRIPT_TEST;
    SB::Execution::execute(thread, code);

    SB::Logger::printf(XORSTR("End Top: %d\n"), lua_gettop(RL));
    SB::Logger::printf(XORSTR("End Top2: %d\n"), lua_gettop(thread));
    return true;
}