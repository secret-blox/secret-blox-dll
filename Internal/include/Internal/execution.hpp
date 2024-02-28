#include "lua.h"
#include "Luau/Compiler.h"
#include "Luau/BytecodeUtils.h"
#include "Luau/BytecodeBuilder.h"

using lua_CFunc = int64_t __fastcall(lua_State*);

class RbxBytecodeEncoder : public Luau::BytecodeEncoder {
    inline void encode(uint32_t* data, size_t count) override {
        // Loop through the instructions.
        for (auto i = 0u; i < count;) {
            // Get the opcode (which is the first byte of the instruction).
            auto& opcode = *reinterpret_cast<uint8_t*>(data + i);

            // Add the instruction length (which could be multiple 32-bit integers).
            i += Luau::getOpLength(LuauOpcode(opcode));

            // Encode the opcode.
            opcode *= 227;
        }
    }
};

namespace SB::Execution {
    typedef enum {
        _0_Anonymous,
        _1_LocalGUI,				// Any action initiated by Roblox Studio or the mouse 
        _2_GameScript,				// Execution of a BaseScript object inside any DataModel
        _3_GameScriptInRobloxPlace,	// Execution of a BaseScript object inside any DataModel, if the place was authored by Roblox
        _4_RobloxGameScript,		// Execution of a BaseScript object written by Roblox inside any DataModel
        _5_CmdLine,					// Any script executed from the Studio command line
        _6_COM,						// Scripts executed via the COM API (usually comes from roblox.com)
        _7_WebService,				// Scripts executed via the Web Service API (usually comes from roblox.com)
        _8_Replicator,				// Receiving data via replication
        COUNT_Identities            // Not a true identity. Used for enumeration
    } Identities;

    extern lua_State* rState; // raw state
    extern lua_State* eState; // execution state
    extern lua_CFunc* taskDefer;
    extern lua_CFunc* coCreate;
    void setup();
    void unload();

    /*
    * Utils for execution
    */

    lua_CFunc* getLibraryFunc(lua_State* L, const char* libName, const char* funcName);
    void loadLibraries(lua_State* L);

    void setIdentity(lua_State* L, Identities identity);
    void setIdentity(uintptr_t userdataBase, Identities identity);

    bool execute(lua_State* L, const char* code);
}