#include "CLuaBase.h"
#include <lua.hpp>

int CLuaBase::lua$meta$Color_eq()
{
#define COMPARE_INDEX(name)                     \
    lua_pushstring(lua_state, name);            \
    lua_gettable(lua_state, 1);                 \
    lua_pushstring(lua_state, name);            \
    lua_gettable(lua_state, 2);                 \
    if (lua_rawequal(lua_state, -1, -2) == 0) { \
        lua_pushboolean(lua_state, false);      \
        return 1;                               \
    }
    COMPARE_INDEX("r")
    COMPARE_INDEX("g")
    COMPARE_INDEX("b")
    COMPARE_INDEX("a")

    lua_pushboolean(lua_state, true);
    return 1;
#undef COMPARE_INDEX
}

int CLuaBase::lua$Color()
{
    int number_of_arguments = lua_gettop(lua_state);

    lua_createtable(lua_state, 0, 4);

    lua_pushstring(lua_state, "r");
    lua_pushvalue(lua_state, 1);
    lua_settable(lua_state, -3);

    lua_pushstring(lua_state, "g");
    lua_pushvalue(lua_state, 2);
    lua_settable(lua_state, -3);

    lua_pushstring(lua_state, "b");
    lua_pushvalue(lua_state, 3);
    lua_settable(lua_state, -3);

    lua_pushstring(lua_state, "a");
    if (number_of_arguments >= 4)
        lua_pushvalue(lua_state, 4);
    else
        lua_pushnumber(lua_state, 255);
    lua_settable(lua_state, -3);

    luaL_setmetatable(lua_state, "Color");

    return 1;
}
