#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/coroutine.create
int CLuaBase::lua$coroutine_create()
{
    auto coroutine = lua_newthread(lua_state);
    coroutine->luabase = this;
    coroutines.push_back(coroutine);
    lua_pushvalue(lua_state, 1);
    lua_xmove(lua_state, coroutine, 1);
    return 1;
}

// https://wiki.facepunch.com/gmod/coroutine.resume
int CLuaBase::lua$coroutine_resume()
{
    int number_of_arguments = lua_gettop(lua_state) - 1;
    auto coroutine = lua_tothread(lua_state, 1);
    lua_xmove(lua_state, coroutine, number_of_arguments);

    auto state_backup = lua_state;
    int coroutine_result = lua_resume(coroutine, number_of_arguments);
    lua_state = state_backup;
    bool success = coroutine_result == LUA_YIELD || coroutine_result == LUA_OK;
    int number_of_returned_values = success ? lua_gettop(coroutine) : 1;
    lua_pushboolean(lua_state, success);
    lua_xmove(coroutine, lua_state, number_of_returned_values);
    return 1 + number_of_returned_values;
}

// https://wiki.facepunch.com/gmod/coroutine.running
int CLuaBase::lua$coroutine_running()
{
    bool is_main_thread = lua_pushthread(lua_state);
    if (is_main_thread) {
        lua_pop(lua_state, 1);
        lua_pushnil(lua_state);
    }
    return 1;
}

// https://wiki.facepunch.com/gmod/coroutine.yield
int CLuaBase::lua$coroutine_yield()
{
    int number_of_arguments = lua_gettop(lua_state);
    return lua_yield(lua_state, number_of_arguments);
}
