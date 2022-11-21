#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/string.EndsWith
int CLuaBase::lua$string_EndsWith()
{
    std::string str = luaL_checkstring(lua_state, 1);
    std::string end = luaL_checkstring(lua_state, 2);

    lua_pushboolean(lua_state, str.ends_with(end));
    return 1;
}

// https://wiki.facepunch.com/gmod/string.Explode
int CLuaBase::lua$string_Explode()
{
    // FIXME: Support patterns.
    std::string separator = luaL_checkstring(lua_state, 1);
    std::string input = luaL_checkstring(lua_state, 2);

    lua_newtable(lua_state);

    size_t next_start = 0;
    double next_table_index = 1;
    while (true) {
        size_t separator_index = input.find(separator, next_start);

        lua_pushnumber(lua_state, next_table_index);
        lua_pushstring(lua_state, input.substr(next_start, separator_index - next_start).c_str());
        lua_settable(lua_state, -3);

        if (separator_index == std::string::npos)
            break;

        next_start = separator_index + separator.length();
        next_table_index++;
    }

    return 1;
}

// https://wiki.facepunch.com/gmod/string.Right
int CLuaBase::lua$string_Right()
{
    std::string str = luaL_checkstring(lua_state, 1);
    size_t num = luaL_checknumber(lua_state, 2);

    lua_pushstring(lua_state, str.substr(str.length() - num).c_str());
    return 1;
}

// https://wiki.facepunch.com/gmod/string.Split
int CLuaBase::lua$string_Split()
{
    lua_pushcfunction(lua_state, lua$string_Explode$entry);
    lua_pushvalue(lua_state, 2);
    lua_pushvalue(lua_state, 1);
    lua_pushboolean(lua_state, false);
    lua_call(lua_state, 3, 1);
    return 1;
}

// https://wiki.facepunch.com/gmod/string.StartWith
int CLuaBase::lua$string_StartWith()
{
    std::string str = luaL_checkstring(lua_state, 1);
    std::string start = luaL_checkstring(lua_state, 2);

    lua_pushboolean(lua_state, str.starts_with(start));
    return 1;
}
