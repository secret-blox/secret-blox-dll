#include "Rbx/instance.hpp"
#include "lua.h"

namespace SB::Rbx {
    class ScriptContext : public Instance {
    public:
        void debugGetLuaState() const;
        lua_State* getLuaState() const;
    };
}