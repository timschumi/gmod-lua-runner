#include "CLuaBase.h"
#include <cassert>
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/coroutine.create
int CLuaBase::lua$coroutine_create(lua_State* lua_state)
{
    assert(lua$coroutine_create$original != -1);
    lua_rawgeti(lua_state, LUA_REGISTRYINDEX, lua$coroutine_create$original);
    lua_pushvalue(lua_state, 1);
    lua_call(lua_state, 1, 1);

    // Register the coroutine in our tracking list.
    coroutines.push_back(lua_tothread(lua_state, -1));

    return 1;
}

// https://wiki.facepunch.com/gmod/coroutine.resume
int CLuaBase::lua$coroutine_resume(lua_State* lua_state)
{
    assert(lua$coroutine_resume$original != -1);
    lua_rawgeti(lua_state, LUA_REGISTRYINDEX, lua$coroutine_resume$original);
    lua_insert(lua_state, 1);
    lua_call(lua_state, lua_gettop(lua_state) - 1, LUA_MULTRET);

    // Restore the previous state, as the in-object pointer might have changed due to running a different thread.
    this->lua_state = lua_state;

    return lua_gettop(lua_state);
}
