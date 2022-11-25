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
