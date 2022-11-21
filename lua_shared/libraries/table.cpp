#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/table.Add
int CLuaBase::lua$table_Add()
{
    luaL_argcheck(lua_state, lua_istable(lua_state, 1), 1, "Expected table");
    luaL_argcheck(lua_state, lua_istable(lua_state, 2), 2, "Expected table");

    lua_pushnil(lua_state);
    while (lua_next(lua_state, 2) != 0) {
        lua_pushnumber(lua_state, lua_objlen(lua_state, 1) + 1);
        lua_insert(lua_state, -2);

        lua_settable(lua_state, 1);
    }

    lua_pushvalue(lua_state, 1);
    return 1;
}

// https://wiki.facepunch.com/gmod/table.Count
int CLuaBase::lua$table_Count()
{
    luaL_argcheck(lua_state, lua_istable(lua_state, 1), 1, "Expected table");

    double count = 0;

    lua_pushnil(lua_state);
    while (lua_next(lua_state, 1) != 0) {
        lua_pop(lua_state, 1);
        count++;
    }

    lua_pushnumber(lua_state, count);
    return 1;
}

// https://wiki.facepunch.com/gmod/table.Inherit
int CLuaBase::lua$table_Inherit()
{
    luaL_argcheck(lua_state, lua_istable(lua_state, 1), 1, "Expected table");
    luaL_argcheck(lua_state, lua_istable(lua_state, 2), 2, "Expected table");

    lua_pushnil(lua_state);
    while (lua_next(lua_state, 2) != 0) {
        lua_pushvalue(lua_state, -2);
        lua_gettable(lua_state, 1);
        bool exists = lua_type(lua_state, -1) != LUA_TNIL;
        lua_pop(lua_state, 1);

        if (exists) {
            lua_pop(lua_state, 1);
            continue;
        }

        lua_pushvalue(lua_state, -2);
        lua_insert(lua_state, -2);
        lua_settable(lua_state, 1);
    }

    lua_pushstring(lua_state, "BaseClass");
    lua_pushvalue(lua_state, 2);
    lua_settable(lua_state, 1);

    lua_pushvalue(lua_state, 1);
    return 1;
}
