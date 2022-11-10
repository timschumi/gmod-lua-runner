#include "CLuaBase.h"
#include <lua.hpp>
#include <regex>

// https://wiki.facepunch.com/gmod/string.EndsWith
int CLuaBase::lua$string_EndsWith()
{
    std::string str = lua_tostring(lua_state, 1);
    std::string end = lua_tostring(lua_state, 2);

    lua_pushboolean(lua_state, str.ends_with(end));
    return 1;
}

// https://wiki.facepunch.com/gmod/string.match
int CLuaBase::lua$string_match()
{
    std::string string = lua_tostring(lua_state, 1);
    // FIXME: Implement support for Lua's pattern matching language.
    // https://wiki.facepunch.com/gmod/Patterns
    std::regex pattern = std::regex(lua_tostring(lua_state, 2));
    double start_position = lua_gettop(lua_state) >= 3 ? lua_tonumber(lua_state, 3) : 1;

    if (start_position > 1) {
        string = string.substr(start_position - 1);
    } else if (start_position < 0) {
        string = string.substr(string.length() + start_position);
    }

    std::smatch matches;
    if (!std::regex_search(string, matches, pattern)) {
        lua_pushnil(lua_state);
        return 1;
    }

    if (matches.size() == 1) {
        lua_pushstring(lua_state, matches[0].str().c_str());
        return 1;
    }

    for (size_t i = 1; i < matches.size(); i++) {
        if (matches[i].matched)
            lua_pushstring(lua_state, matches[i].str().c_str());
        else
            lua_pushnil(lua_state);
    }

    return matches.size() - 1;
}

// https://wiki.facepunch.com/gmod/string.Split
int CLuaBase::lua$string_Split()
{
    std::string input = lua_tostring(lua_state, 1);
    std::string separator = lua_tostring(lua_state, 2);

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

// https://wiki.facepunch.com/gmod/string.StartWith
int CLuaBase::lua$string_StartWith()
{
    std::string str = lua_tostring(lua_state, 1);
    std::string start = lua_tostring(lua_state, 2);

    lua_pushboolean(lua_state, str.starts_with(start));
    return 1;
}
