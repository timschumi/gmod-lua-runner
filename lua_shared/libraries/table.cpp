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
