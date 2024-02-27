#include "lua.h"
#include "Luau/Compiler.h"
#include "Luau/BytecodeUtils.h"
#include "Luau/BytecodeBuilder.h"

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
    extern lua_State* mainState;
    void setup();
    void unload();
}