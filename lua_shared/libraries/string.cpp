#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/string.EndsWith
int CLuaBase::lua$string_EndsWith(lua_State* lua_state)
{
    std::string str = luaL_checkstring(lua_state, 1);
    std::string end = luaL_checkstring(lua_state, 2);

    lua_pushboolean(lua_state, str.ends_with(end));
    return 1;
}

// https://wiki.facepunch.com/gmod/string.Explode
int CLuaBase::lua$string_Explode(lua_State* lua_state)
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

// https://wiki.facepunch.com/gmod/string.Replace
int CLuaBase::lua$string_Replace(lua_State* lua_state)
{
    std::string str = luaL_checkstring(lua_state, 1);
    std::string find = luaL_checkstring(lua_state, 2);
    size_t find_len = find.length();
    std::string replace = luaL_checkstring(lua_state, 3);
    std::string result;

    size_t offset = 0;
    while (true) {
        size_t chunk_start = offset;
        offset = str.find(find, offset);

        if (offset == std::string::npos) {
            result.append(str.substr(chunk_start));
            break;
        }

        result.append(str.substr(chunk_start, offset - chunk_start));

        result.append(replace);
        offset += find_len;
    }

    lua_pushstring(lua_state, result.c_str());
    return 1;
}

// https://wiki.facepunch.com/gmod/string.Right
int CLuaBase::lua$string_Right(lua_State* lua_state)
{
    std::string str = luaL_checkstring(lua_state, 1);
    size_t str_length = str.length();
    double num = luaL_checknumber(lua_state, 2);

    if (num < 1)
        num = str_length + num + 1;

    if (num < 0)
        num = 0;
    else if (num > str_length)
        num = str_length;

    lua_pushstring(lua_state, str.substr(str_length - num).c_str());
    return 1;
}

// https://wiki.facepunch.com/gmod/string.Split
int CLuaBase::lua$string_Split(lua_State* lua_state)
{
    lua_pushcfunction(lua_state, lua$string_Explode$entry);
    lua_pushvalue(lua_state, 2);
    lua_pushvalue(lua_state, 1);
    lua_pushboolean(lua_state, false);
    lua_call(lua_state, 3, 1);
    return 1;
}

// https://wiki.facepunch.com/gmod/string.StartWith
int CLuaBase::lua$string_StartWith(lua_State* lua_state)
{
    std::string str = luaL_checkstring(lua_state, 1);
    std::string start = luaL_checkstring(lua_state, 2);

    lua_pushboolean(lua_state, str.starts_with(start));
    return 1;
}
