#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/string.EndsWith
int CLuaBase::lua$string_EndsWith()
{
    std::string str = lua_tostring(lua_state, 1);
    std::string end = lua_tostring(lua_state, 2);

    lua_pushboolean(lua_state, str.ends_with(end));
    return 1;
}
