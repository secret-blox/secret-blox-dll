#include "Internal/execution.hpp"
#include "Internal/logger.hpp"
#include "Internal/utils.hpp"

#include "Security/xor.hpp"

#include "lapi.h"
#include "lstate.h"

#include "offsets.hpp"

lua_State* SB::Execution::rState = nullptr;
lua_State* SB::Execution::eState = nullptr;
lua_CFunc* SB::Execution::taskDefer = nullptr;
lua_CFunc* SB::Execution::coCreate = nullptr;
/*
uintptr_t __fastcall pseudo2addrHook(lua_State* L, int idx)
{
    // unhook function
    // ...
    // create new thread from the current state
    SB::Execution::mainState = lua_newthread(L);
    lua_pop(L, 1); // clean stack
    // return normal pseudo2addr
    return 0; // TODO
}
*/



void SB::Execution::setup()
{
    // already setup
    if (rState && eState)
        return;
    // TODO
}

void SB::Execution::unload()
{
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

void SB::Execution::setIdentity(lua_State* L, Identities identity)
{
    uintptr_t userdataBase = reinterpret_cast<uintptr_t>(L->userdata);
    return setIdentity(userdataBase, identity);
}
void SB::Execution::setIdentity(uintptr_t userdataBase, Identities identity)
{
    if (!userdataBase)
        return;
    *(uintptr_t*)(userdataBase + SB::Offsets::Luau::Userdata::identity) = identity;
    *(uintptr_t*)(userdataBase + SB::Offsets::Luau::Userdata::capFlags) = 0xFF3F;
    *(uintptr_t*)(userdataBase + SB::Offsets::Luau::Userdata::capFlags2) = 0xFF3F;
}

RbxBytecodeEncoder encoder;
bool SB::Execution::execute(lua_State *L, const char *code)
{
    if (!taskDefer)
    {
        SB::Logger::printf(XORSTR("taskDefer not set\n"));
        return false;
    }

    std::string bytecode = Luau::compile(code, {}, {}, &encoder);
    if (bytecode.at(0) == 0)
	{
        SB::Logger::printf(
            XORSTR("Failed to compile: %s\n"), 
            (const char*)(bytecode.c_str() + 1)
        );
		return false;
	}

    std::string chunkName = "=" + Utils::randomString(8);
    if (luau_load(L, chunkName.c_str(), bytecode.c_str(), bytecode.size(), 0) == LUA_OK)
    {
        auto cl = (Closure*)lua_topointer(L, -1);
        Proto* p = cl->l.p;
        p->userdata = L->userdata;

        taskDefer(L);
        lua_pop(L, 1);
    }
    else
    {
        SB::Logger::printf(
            XORSTR("Failed to load: %s\n"), 
            lua_tostring(L, -1)
        );
        lua_pop(L, 1);
    }


    return true;
}