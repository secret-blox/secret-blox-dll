#include "lua.h"


namespace SB::Execution {
    extern lua_State* mainState;
    void setup();
    void unload();
}