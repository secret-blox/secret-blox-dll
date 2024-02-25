#include "Internal/execution.hpp"
#include "Internal/detour.hpp"

lua_State* SB::Execution::mainState = nullptr;
uintptr_t __fastcall pseudo2addrHook(lua_State* L, int idx)
{
    // create new thread from the current state
    SB::Execution::mainState = lua_newthread(L);
    lua_pop(L, 1); // clean stack
    // unhook function
    // return normal pseudo2addr
    return 0; // TODO
}

void SB::Execution::setup()
{
    // 
}

void SB::Execution::unload()
{
}