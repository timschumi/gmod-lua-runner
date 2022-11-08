#include "CLuaBase.h"
#include <lua.hpp>

int CLuaBase::lua$timer_create()
{
    std::string identifier = lua_tostring(lua_state, 1);
    lua_Number delay = lua_tonumber(lua_state, 2);
    lua_Number repetitions = lua_tonumber(lua_state, 3);
    lua_pushvalue(lua_state, 4);
    int function_ref = luaL_ref(lua_state, LUA_REGISTRYINDEX);

    if (timers.contains(identifier))
        luaL_unref(lua_state, LUA_REGISTRYINDEX, timers[identifier].function);

    timers[identifier] = { delay, repetitions, function_ref, delay };

    return 0;
}
