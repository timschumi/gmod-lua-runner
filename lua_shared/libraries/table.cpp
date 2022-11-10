#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/table.Add
int CLuaBase::lua$table_Add()
{
    lua_pushnil(lua_state);
    while (lua_next(lua_state, 2) != 0) {
        // Duplicate the key, since we will still need it for the next iteration.
        lua_pushvalue(lua_state, -2);
        lua_insert(lua_state, -3);

        lua_settable(lua_state, 1);
    }
    return 1;
}

// https://wiki.facepunch.com/gmod/table.Count
int CLuaBase::lua$table_Count()
{
    double count = 0;

    lua_pushnil(lua_state);
    while (lua_next(lua_state, 1) != 0) {
        lua_pop(lua_state, 1);
        count++;
    }

    lua_pushnumber(lua_state, count);
    return 1;
}

// https://wiki.facepunch.com/gmod/table.insert
int CLuaBase::lua$table_insert()
{
    // FIXME: Actually shift down conflicting entries.
    if (lua_gettop(lua_state) >= 3) {
        lua_pushvalue(lua_state, 2);
        lua_pushvalue(lua_state, 3);
    } else {
        lua_pushnumber(lua_state, lua_objlen(lua_state, 1) + 1);
        lua_pushvalue(lua_state, 2);
    }

    lua_pushvalue(lua_state, -2);
    lua_insert(lua_state, -2);

    lua_settable(lua_state, 1);

    return 1;
}

// https://wiki.facepunch.com/gmod/table.remove
int CLuaBase::lua$table_remove()
{
    size_t table_length = lua_objlen(lua_state, 1);
    double index = lua_gettop(lua_state) >= 2 ? lua_tonumber(lua_state, 2) : table_length;

    if (index < 1 || index > table_length)
        return 0;

    lua_pushnumber(lua_state, index);
    lua_gettable(lua_state, 1);

    lua_pushnumber(lua_state, index);
    lua_pushnil(lua_state);
    lua_settable(lua_state, 1);

    return 1;
}
