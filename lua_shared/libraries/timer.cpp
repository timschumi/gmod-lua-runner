#include "CLuaBase.h"
#include <lua.hpp>

int CLuaBase::lua$timer_create()
{
    char const* identifier = lua_tostring(lua_state, 1);
    lua_Number delay = lua_tonumber(lua_state, 2);
    lua_Number repetitions = lua_tonumber(lua_state, 3);
    lua_CFunction func = lua_tocfunction(lua_state, 4);

    // FIXME: Implement timers.
    printf("FIXME: timer.Create('%s', %f, %f, %p) is not implemented.\n", identifier, delay, repetitions, func);

    return 0;
}
