#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/timer.Create
int CLuaBase::lua$timer_Create(lua_State* lua_state)
{
    std::string identifier = luaL_checkstring(lua_state, 1);
    lua_Number delay = luaL_checknumber(lua_state, 2);
    lua_Number repetitions = luaL_checknumber(lua_state, 3);
    luaL_argcheck(lua_state, lua_isfunction(lua_state, 4), 4, "Expected function");
    lua_pushvalue(lua_state, 4);
    int function_ref = luaL_ref(lua_state, LUA_REGISTRYINDEX);

    if (timers.contains(identifier))
        luaL_unref(lua_state, LUA_REGISTRYINDEX, timers[identifier].function);

    timers[identifier] = { delay, repetitions != 0 ? std::optional { repetitions } : std::nullopt, function_ref, delay };

    return 0;
}

// https://wiki.facepunch.com/gmod/timer.Exists
int CLuaBase::lua$timer_Exists(lua_State* lua_state)
{
    std::string identifier = luaL_checkstring(lua_state, 1);
    lua_pushboolean(lua_state, timers.contains(identifier));
    return 1;
}

// https://wiki.facepunch.com/gmod/timer.Remove
int CLuaBase::lua$timer_Remove(lua_State* lua_state)
{
    std::string identifier = luaL_checkstring(lua_state, 1);

    if (!timers.contains(identifier))
        return 0;

    luaL_unref(lua_state, LUA_REGISTRYINDEX, timers[identifier].function);
    timers.erase(identifier);
    return 0;
}

// https://wiki.facepunch.com/gmod/timer.Simple
int CLuaBase::lua$timer_Simple(lua_State* lua_state)
{
    lua_Number delay = luaL_checknumber(lua_state, 1);
    luaL_argcheck(lua_state, lua_isfunction(lua_state, 2), 2, "Expected function");
    lua_pushvalue(lua_state, 2);
    int function_ref = luaL_ref(lua_state, LUA_REGISTRYINDEX);

    simple_timers.push_back({ function_ref, delay });

    return 0;
}
