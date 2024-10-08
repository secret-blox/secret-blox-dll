#include "Execution/execution.hpp"
#include "Internal/logger.hpp"
#include "Internal/utils.hpp"

#include "Rbx/taskscheduler.hpp"
#include "Rbx/scriptcontext.hpp"

#include "Security/xor.hpp"

#include "lualib.h"
#include "lapi.h"
#include "lstate.h"

#include "offsets.hpp"

bool SB::Execution::ready = false;
lua_State* SB::Execution::rState = nullptr;
lua_State* SB::Execution::eState = nullptr;
int SB::Execution::eStateRef = 0;

lua_CFunc* SB::Execution::taskDefer = nullptr;
lua_CFunc* SB::Execution::coCreate = nullptr;

void SB::Execution::setup()
{
    // already setup
    if (ready)
        return;

    const auto taskScheduler = SB::Rbx::TaskScheduler::get();
    const auto luaGc = taskScheduler.getJobByName("LuaGc");
    if (!luaGc.has_value())
    {
        SB::Logger::printf(XORSTR("Failed to get LuaGc job\n"));
        return;
    }

    Rbx::ScriptContext scriptContext = luaGc->getScriptContext();
    Execution::rState = scriptContext.getLuaState();
    SB::Execution::coCreate = SB::Execution::getLibraryFunc(rState, "coroutine", "create");
    SB::Execution::taskDefer = SB::Execution::getLibraryFunc(rState, "task", "defer");
    if (!coCreate || !taskDefer)
    {
        SB::Logger::printf(XORSTR("Failed to get coroutine.create/task.defer\n"));
        return;
    }
    Execution::eState = createThread(rState);
    lua_pushthread(eState);
    Execution::eStateRef = lua_ref(eState, -1); // create ref to eState
    lua_pop(eState, 1);
    luaL_sandboxthread(eState);
    loadLibraries(eState);
    setIdentity(eState, _8_Replicator);
    ready = true;
}

void SB::Execution::unload()
{
    if (eStateRef)
    {
        lua_unref(eState, eStateRef);
        eStateRef = 0;
    }
    return;
}

void SB::Execution::loadLibraries(lua_State* L)
{
    // TODO
}

lua_CFunc* SB::Execution::getLibraryFunc(lua_State *L, const char *libName, const char *funcName)
{
    auto libTT = lua_rawgetfield(L, LUA_GLOBALSINDEX, libName);
    if (libTT != LUA_TTABLE)
    {
        lua_pop(L, 1);
        return nullptr;
    }
    auto funcTT = lua_rawgetfield(L, -1, funcName);
    if (funcTT != LUA_TFUNCTION)
    {
        lua_pop(L, 2);
        return nullptr;
    }
    auto func = *luaA_toobject(L, -1);
    lua_pop(L, 2);
    return reinterpret_cast<lua_CFunc*>((lua_CFunction)func.value.gc->cl.c.f);
}

uintptr_t sbCapabilities = 0xEFFFFFFFFFFFFFFF;
void SB::Execution::setIdentity(lua_State* L, Identities identity)
{
    uintptr_t userdataBase = reinterpret_cast<uintptr_t>(L->userdata);
    *(uintptr_t*)(userdataBase + SB::Offsets::Luau::Userdata::identity) = identity;
    *(uintptr_t*)(userdataBase + SB::Offsets::Luau::Userdata::capFlags) = sbCapabilities;
}

void SB::Execution::setCapabilities(Proto* proto, uintptr_t* capFlags)
{
    if (!proto)
        return;
    proto->userdata = capFlags;
    for (int i = 0; i < proto->sizep; ++i)
        setCapabilities(proto->p[i], capFlags);
}

lua_State* SB::Execution::createThread(lua_State* L) 
{
    if (!coCreate)
    {
        SB::Logger::printf(XORSTR("coCreate not set\n"));
        return nullptr;
    }
    // push fake object on stack
    L->top->tt = LUA_TFUNCTION;
    L->top++;
    SB::Execution::coCreate(L);
    auto L1TV = *luaA_toobject(L, -1);
    if (L1TV.tt != LUA_TTHREAD)
    {
        SB::Logger::printf(XORSTR("coCreate did not return a thread\n"));
        lua_pop(L, 1);
        return nullptr;
    }
    auto L1 = reinterpret_cast<lua_State*>(L1TV.value.gc);
    lua_pop(L1, 1);
    lua_pop(L, 2);
    return L1;
}

RbxBytecodeEncoder encoder;
bool SB::Execution::execute(std::string code)
{
    return execute(eState, code);
}
bool SB::Execution::execute(lua_State *L, std::string code)
{
    if (!ready || !taskDefer)
        return false;

    auto thread = createThread(L);
    luaL_sandboxthread(thread);

    std::string compCode = "script=Instance.new(\"LocalScript\");\t" + code;
    std::string bytecode = Luau::compile(compCode, {}, {}, &encoder);
    if (bytecode.at(0) == 0)
	{
        SB::Logger::printf(
            XORSTR("Failed to compile: %s\n"), 
            (const char*)(bytecode.c_str() + 1)
        );
		return false;
	}

    std::string chunkName = "=" + Utils::randomString(8);
    if (luau_load(thread, chunkName.c_str(), bytecode.c_str(), bytecode.size(), 0) == LUA_OK)
    {
        auto cl = (Closure*)lua_topointer(thread, -1);
        Proto* p = cl->l.p;
        setCapabilities(p, &sbCapabilities);

        taskDefer(thread);
        lua_pop(thread, 1);
    }
    else
    {
        SB::Logger::printf(
            XORSTR("Failed to load: %s\n"), 
            lua_tostring(thread, -1)
        );
        lua_pop(thread, 1);
    }
    SB::Logger::printf(XORSTR("Executed, top=%d\n"), lua_gettop(thread));

    return true;
}