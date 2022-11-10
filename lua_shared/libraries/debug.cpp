#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/debug.getinfo
int CLuaBase::lua$debug_getinfo()
{
    std::string fields = lua_gettop(lua_state) >= 2 ? lua_tostring(lua_state, 2) : "flnSu";
    lua_Debug ar {};
    if (lua_type(lua_state, 1) == LUA_TNUMBER) {
        lua_getstack(lua_state, lua_tonumber(lua_state, 1), &ar);
    } else {
        fields = ">" + fields;
        lua_pushvalue(lua_state, 1);
    }

    if (lua_getinfo(lua_state, fields.c_str(), &ar) == 0) {
        lua_pushnil(lua_state);
        return 1;
    }

    lua_newtable(lua_state);

    // Note: LuaJIT internals: 'f' gets processed first and 'L' last, so we should pop those in the reverse order.
    if (fields.find('L') != std::string::npos) {
        lua_pushstring(lua_state, "activelines");
        lua_pushvalue(lua_state, -3);
        lua_settable(lua_state, -3);
        lua_remove(lua_state, -2);
    }

    if (fields.find('f') != std::string::npos) {
        lua_pushstring(lua_state, "func");
        lua_pushvalue(lua_state, -3);
        lua_settable(lua_state, -3);
        lua_remove(lua_state, -2);
    }

    if (fields.find('l') != std::string::npos) {
        lua_pushstring(lua_state, "currentline");
        lua_pushnumber(lua_state, ar.currentline);
        lua_settable(lua_state, -3);
    }

    if (fields.find('n') != std::string::npos) {
        lua_pushstring(lua_state, "name");
        lua_pushstring(lua_state, ar.name);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "namewhat");
        lua_pushstring(lua_state, ar.namewhat);
        lua_settable(lua_state, -3);
    }

    if (fields.find('S') != std::string::npos) {
        lua_pushstring(lua_state, "lastlinedefined");
        lua_pushnumber(lua_state, ar.lastlinedefined);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "linedefined");
        lua_pushnumber(lua_state, ar.linedefined);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "short_src");
        lua_pushstring(lua_state, ar.short_src);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "source");
        lua_pushstring(lua_state, ar.source);
        lua_settable(lua_state, -3);

        lua_pushstring(lua_state, "what");
        lua_pushstring(lua_state, ar.what);
        lua_settable(lua_state, -3);
    }

    if (fields.find('u') != std::string::npos) {
        // FIXME: Missing isvararg and nparams.
        lua_pushstring(lua_state, "nups");
        lua_pushnumber(lua_state, ar.nups);
        lua_settable(lua_state, -3);
    }

    return 1;
}
