#include <cmath>
#include <memory>

#include "Rbx/scriptcontext.hpp"
#include "Internal/memory.hpp"
#include "Internal/logger.hpp"
#include "lstate.h"

#define __PAIR64__(high, low)   (((uint64_t) (high) << 32) | (uint32_t)(low))

void SB::Rbx::ScriptContext::debugGetLuaState() const
{
    if (!baseAddress)
        return;
    
    uintptr_t obfBase4 = baseAddress + 0x110;
    SB::Logger::printf("obfBase4: %p\n", obfBase4);
    uintptr_t obfBase5 = obfBase4 + 0x140 + 32 + 0x88; // remove 0x140 if you want a lower identity state
    SB::Logger::printf("obfBase5: %p\n", obfBase5);
    uintptr_t lstate = __PAIR64__(*reinterpret_cast<uint32_t*>(obfBase5 + 4) + (int)obfBase5, (int)obfBase5 + *reinterpret_cast<uint32_t*>(obfBase5));
    SB::Logger::printf("lstate: %p\n", lstate);
}

lua_State *SB::Rbx::ScriptContext::getLuaState() const
{
    if (!baseAddress)
        return nullptr;
    // TODO: avoid hardcoding offsets once we can consider the method stable
    uintptr_t statesArray = baseAddress + 0x110;
    uintptr_t obfState = statesArray + 0x140 + 32 + 0x88; // remove 0x140 if you want a lower identity state
    auto state = reinterpret_cast<lua_State*>(
        __PAIR64__(*reinterpret_cast<uint32_t*>(obfState + 4) + (int)obfState, (int)obfState + *reinterpret_cast<uint32_t*>(obfState))
    );
    // check if state is valid
    std::unique_ptr heap = std::make_unique<char[]>(0x100);
    const size_t diff = reinterpret_cast<uintptr_t>(heap.get()) - reinterpret_cast<uintptr_t>(state);
    if (std::abs(static_cast<long long>(diff)) >= 0x63EF0000)
    {
        SB::Logger::printf("lua_State out of heap boundaries: %p, diff: %llx\n", state, diff);
        return nullptr;
    }
    else if (state->tt != LUA_TTHREAD)
    {
        SB::Logger::printf("Invalid lua_State->tt, got %d, expected %d\n", state->tt, LUA_TTHREAD);
        return nullptr;
    }
    return state;
}