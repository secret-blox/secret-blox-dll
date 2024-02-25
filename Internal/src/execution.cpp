#include "Internal/execution.hpp"

lua_State* SB::Execution::mainState = nullptr;

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
    // 
}

void SB::Execution::unload()
{
}