#include "Rbx/instance.hpp"
#include "lua.h"

namespace SB::Rbx {
    class ScriptContext : public Instance {
    public:
        lua_State* getLuaState() const;
    };
}