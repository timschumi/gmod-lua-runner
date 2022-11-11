#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/concommand.Add
int CLuaBase::lua$concommand_Add()
{
    char const* name = lua_tostring(lua_state, 1);
    lua_pushvalue(lua_state, 2);
    int callback = luaL_ref(lua_state, LUA_REGISTRYINDEX);
    // Note: Autocomplete is ignored.
    std::string help_text = lua_gettop(lua_state) >= 4 ? lua_tostring(lua_state, 4) : "nil";
    int flags = lua_gettop(lua_state) >= 5 ? lua_tonumber(lua_state, 5) : 0;

    if (concommands.contains(name)) {
        luaL_unref(lua_state, LUA_REGISTRYINDEX, concommands[name].callback);
    }

    concommands[name] = { callback, help_text, flags };

    return 0;
}
