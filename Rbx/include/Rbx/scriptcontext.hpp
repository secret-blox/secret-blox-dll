#pragma once
#include "Rbx/instance.hpp"
#include "lua.h"

namespace SB::Rbx {
    class ScriptContext : public Instance {
    public:
        ScriptContext(uintptr_t _baseAddress) : Instance(_baseAddress) {};
        ScriptContext(const Instance& instance) : Instance(instance) {}

        void debugGetLuaState() const;
        lua_State* getLuaState() const;
    };
}