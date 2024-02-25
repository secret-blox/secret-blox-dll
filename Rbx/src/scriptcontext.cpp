#include "Rbx/scriptcontext.hpp"
#include "Internal/memory.hpp"
#include "Internal/logger.hpp"

using getStatesPhaseCC = uintptr_t __fastcall(uintptr_t);

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
    



    return nullptr;
}