#include "CLuaBase.h"
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/debug.getinfo
int CLuaBase::lua$debug_getinfo()
{
    std::string fields = lua_gettop(lua_state) >= 2 ? luaL_checkstring(lua_state, 2) : "flnSu";
    lua_Debug ar {};
    if (lua_type(lua_state, 1) == LUA_TNUMBER) {
        lua_getstack(lua_state, lua_tonumber(lua_state, 1), &ar);
    } else {
        fields = ">" + fields;
        luaL_argcheck(lua_state, lua_isfunction(lua_state, 1), 1, "Expected function");
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

// https://wiki.facepunch.com/gmod/debug.getlocal
int CLuaBase::lua$debug_getlocal()
{
    // FIXME: Support for passing functions to get the parameter names.
    bool passed_thread = lua_gettop(lua_state) >= 3;
    if (passed_thread)
        luaL_argcheck(lua_state, lua_isthread(lua_state, 1), 1, "Expected thread");
    lua_State* thread = passed_thread ? lua_tothread(lua_state, 1) : lua_state;
    int level = luaL_checknumber(lua_state, passed_thread ? 2 : 1);
    int index = luaL_checknumber(lua_state, passed_thread ? 3 : 2);

    lua_Debug ar {};
    lua_getstack(thread, level, &ar);
    char const* name = lua_getlocal(lua_state, &ar, index);

    if (name != nullptr) {
        lua_pushstring(lua_state, name);
        lua_insert(lua_state, -2);
    } else {
        lua_pushnil(lua_state);
        lua_pushnil(lua_state);
    }

    return 2;
}
