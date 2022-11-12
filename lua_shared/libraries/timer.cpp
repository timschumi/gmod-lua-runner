#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/timer.Create
int CLuaBase::lua$timer_Create()
{
    std::string identifier = lua_tostring(lua_state, 1);
    lua_Number delay = lua_tonumber(lua_state, 2);
    lua_Number repetitions = lua_tonumber(lua_state, 3);
    lua_pushvalue(lua_state, 4);
    int function_ref = luaL_ref(lua_state, LUA_REGISTRYINDEX);

    if (timers.contains(identifier))
        luaL_unref(lua_state, LUA_REGISTRYINDEX, timers[identifier].function);

    timers[identifier] = { delay, repetitions != 0 ? std::optional { repetitions } : std::nullopt, function_ref, delay };

    return 0;
}

// https://wiki.facepunch.com/gmod/timer.Remove
int CLuaBase::lua$timer_Remove()
{
    std::string identifier = lua_tostring(lua_state, 1);

    if (!timers.contains(identifier))
        return 0;

    luaL_unref(lua_state, LUA_REGISTRYINDEX, timers[identifier].function);
    timers.erase(identifier);
    return 0;
}
