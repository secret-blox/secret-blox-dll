#include "Internal/test.hpp"
#include "Internal/logger.hpp"
#include "Internal/memory.hpp"

#include "Rbx/scriptcontext.hpp"
#include "Rbx/taskscheduler.hpp"

#include "Execution/scheduler.hpp"
#include "Execution/execution.hpp"

#include "Security/xor.hpp"

#include "lua.h"
#include "lapi.h"
#include "lstate.h"
#include "lualib.h"

#include "sb_scripts.hpp"

#define SB_ASSERT(expr) if (!(expr)) { SB::Logger::printf("Assertion failed, %s, at line %d. \n", #expr, __LINE__); return false; }

bool SB::Test::run()
{
    // exception handler will caugh this in case of failure
    const auto taskScheduler = SB::Rbx::TaskScheduler::get();
    SB::Logger::printf(XORSTR("TaskScheduler: %p\n"), taskScheduler.baseAddress);

    // verify jobs count
    const auto jobs = taskScheduler.getJobs();
    const auto jobsSize = jobs.size();
    SB_ASSERT(jobsSize >= 20 && jobsSize <= 80); // jobs size should be between 20 and 80
    SB::Logger::printf(XORSTR("Jobs: %d\n"), jobs.size());

    // verify LuaGc existence
    const auto luaGc = taskScheduler.getJobByName("LuaGc");
    SB_ASSERT(luaGc.has_value()); // LuaGc job should exist

    // verify script context integrity
    Rbx::ScriptContext scriptContext = luaGc->getScriptContext();
    SB_ASSERT(scriptContext.getBaseAddress() != 0);
    Rbx::Instance dataModel = scriptContext.getParent();
    SB_ASSERT(dataModel.getBaseAddress() != 0);
    SB_ASSERT(scriptContext.getClassName() == "ScriptContext");

    // verify nilobject & dummynode
    SB::Logger::printf(XORSTR("Testing nilobject & dummynode\n"));
    SB_ASSERT(rluaO_nilobject->tt == LUA_TNIL);
    SB_ASSERT(rluaH_dummynode->key.tt == LUA_TNIL);
    SB_ASSERT(rluaH_dummynode->val.tt == LUA_TNIL);

    // retrive lua_State and verify integrity
    lua_State* RL = scriptContext.getLuaState();
    SB_ASSERT(RL != nullptr);
    SB::Logger::printf(XORSTR("Roblox lua_State: %p\n"), RL);
    SB_ASSERT(RL->tt == LUA_TTHREAD);
    SB::Logger::printf(XORSTR("stackSize: %d\n"), (int)RL->stacksize);

    // verify global table
    auto gt = RL->gt;
    SB_ASSERT(gt != nullptr);
    SB_ASSERT(gt->tt == LUA_TTABLE);
    
    // verify state & tvalue integrity by grabbing globaltable
    const auto gt2 = luaA_toobject(RL, LUA_GLOBALSINDEX);
    SB_ASSERT((uintptr_t)gt2->value.gc == (uintptr_t)gt);
    SB::Logger::printf(XORSTR("Start Top: %d\n"), lua_gettop(RL));

    // SETUP EXECUTION
    SB::Execution::rState = RL;

    // verify global state integrity
    SB::Logger::printf(XORSTR("Testing global state\n"));
    TString* idxTS = RL->global->tmname[TM_INDEX];
    SB::Logger::printf(XORSTR("idxTT: %d\n"), idxTS->tt);
    SB_ASSERT(idxTS->tt == LUA_TSTRING);
    SB::Logger::printf(XORSTR("idxTS: %s\n"), idxTS->data);
    SB_ASSERT(strcmp(idxTS->data, "__index") == 0);

    TString* idxTS = RL->global->ttname[LUA_TFUNCTION];
    SB::Logger::printf(XORSTR("idxTT: %d\n"), idxTS->tt);
    SB_ASSERT(idxTS->tt == LUA_TSTRING);
    SB::Logger::printf(XORSTR("idxTS: %s\n"), idxTS->data);
    SB_ASSERT(strcmp(idxTS->data, "function") == 0);

    // verify table struct & tstring
    lua_rawgetfield(RL, LUA_GLOBALSINDEX, "_VERSION");
    auto version = *luaA_toobject(RL, -1);
    lua_pop(RL, 1);
    SB_ASSERT(version.tt == LUA_TSTRING);
    // version->value.gc->ts.data;
    SB_ASSERT(strcmp(version.value.gc->ts.data, "Luau") == 0);

     // check table metatable by looking at game metatable
    lua_getglobal(RL, "game");
    auto game = *luaA_toobject(RL, -1);
    SB_ASSERT(game.tt == LUA_TUSERDATA);
    SB_ASSERT(game.value.gc->u.metatable != nullptr);
    SB_ASSERT(game.value.gc->u.metatable->tt == LUA_TTABLE);
    SB_ASSERT(game.value.gc->u.metatable->readonly == 1);
    lua_pop(RL, 1);

    // try raw set field
    lua_pushnumber(RL, 2069);
    lua_rawsetfield(RL, LUA_GLOBALSINDEX, "epicnumber");
    lua_rawgetfield(RL, LUA_GLOBALSINDEX, "epicnumber");
    auto epicnumber = *luaA_toobject(RL, -1);
    lua_pop(RL, 1);
    SB_ASSERT(epicnumber.tt == LUA_TNUMBER);
    SB_ASSERT(epicnumber.value.n == 2069);

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

    // verify thread creation
    auto thread = SB::Execution::createThread(RL);
    SB_ASSERT(thread != nullptr);
    SB_ASSERT(thread != RL);
    SB_ASSERT(lua_gettop(thread) == 0);

    // verify sandboxing
    luaL_sandboxthread(thread);    
    SB_ASSERT((Table*)thread->gt != (Table*)RL->gt)
    lua_getmetatable(thread, LUA_GLOBALSINDEX);
    auto newGt = *luaA_toobject(thread, -1);
    SB_ASSERT(newGt.tt == LUA_TTABLE);
    SB_ASSERT(newGt.value.gc->h.readonly == 1);
    lua_getfield(thread, -1, "__index");
    auto gtIndex = *luaA_toobject(thread, -1);
    lua_pop(thread, 2);
    SB_ASSERT(gtIndex.tt == LUA_TTABLE);
    SB_ASSERT((Table*)gtIndex.value.gc == RL->gt);

    // verify sandbox env
    lua_getglobal(thread, "game");
    auto game2 = *luaA_toobject(thread, -1);
    lua_pop(thread, 1);
    SB::Logger::printf(XORSTR("game2: %p\n"), game2.value.gc );
    SB::Logger::printf(XORSTR("game2tt: %d\n"), game2.tt );
    SB_ASSERT(game2.tt == LUA_TUSERDATA);
    SB_ASSERT(game2.value.gc == game.value.gc);

    // SETUP EXECUTION 3
    SB::Execution::eState = thread;
    lua_pushthread(thread);
    SB::Execution::eStateRef = lua_ref(thread, -1); // create ref to eState
    lua_pop(thread, 1);

    // verify lua_ref
    lua_getref(thread, SB::Execution::eStateRef);
    auto eStateTV = *luaA_toobject(thread, -1);
    lua_pop(thread, 1);
    SB_ASSERT(eStateTV.tt == LUA_TTHREAD);
    SB_ASSERT((lua_State*)eStateTV.value.gc == thread);

    SB::Execution::setIdentity(thread, SB::Execution::_8_Replicator);
    SB::Execution::ready = true;

    SB::Logger::printf(XORSTR("eState: %p\n"), (uintptr_t)thread);
    SB::Logger::printf(XORSTR("eStateRef: %d\n"), SB::Execution::eStateRef);
    SB::Logger::printf(XORSTR("End Top: %d\n"), lua_gettop(RL));
    SB::Logger::printf(XORSTR("End Top2: %d\n"), lua_gettop(thread));
    
    auto code = SB_SCRIPT_TEST;
    SB::Scheduler::queueScript(code);
    
    return true;
}