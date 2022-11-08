#include "CLuaBase.h"
#include <lua.hpp>

int CLuaBase::lua$hook_add()
{
    std::string event_name = lua_tostring(lua_state, 1);
    std::string identifier = lua_tostring(lua_state, 2);

    // Note: Closures can't be transferred into a raw C value, so we'll have to use references.
    lua_pushvalue(lua_state, 3);
    int func = luaL_ref(lua_state, LUA_REGISTRYINDEX);

    if (!registered_hooks.contains(event_name))
        registered_hooks[event_name] = {};

    if (registered_hooks[event_name].contains(identifier))
        luaL_unref(lua_state, LUA_REGISTRYINDEX, registered_hooks[event_name][identifier]);

    // FIXME: Support table/object identifiers.
    registered_hooks[event_name][identifier] = func;

    return 0;
}

int CLuaBase::lua$hook_call()
{
    std::string event_name = lua_tostring(lua_state, 1);
    // FIXME: Check on the gamemode table.
    lua_pushvalue(lua_state, 2);
    int gamemode_table_ref = luaL_ref(lua_state, LUA_REGISTRYINDEX);
    int number_of_arguments = lua_gettop(lua_state) - 2;

    if (!registered_hooks.contains(event_name))
        return 0;

    for (auto const& hook : registered_hooks[event_name]) {
        int stack_top_without_args = lua_gettop(lua_state);

        lua_rawgeti(lua_state, LUA_REGISTRYINDEX, hook.second);
        for (int i = 1; i <= number_of_arguments; i++)
            lua_pushvalue(lua_state, 2 + i);
        lua_call(lua_state, number_of_arguments, LUA_MULTRET);

        int number_of_returned_values = lua_gettop(lua_state) - stack_top_without_args;
        if (number_of_returned_values > 0 && lua_type(lua_state, -number_of_returned_values)) {
            luaL_unref(lua_state, LUA_REGISTRYINDEX, gamemode_table_ref);
            return number_of_returned_values;
        }
    }

    luaL_unref(lua_state, LUA_REGISTRYINDEX, gamemode_table_ref);
    return 0;
}

int CLuaBase::lua$hook_get_table()
{
    lua_createtable(lua_state, 0, 0);

    for (auto const& event : registered_hooks) {
        lua_pushstring(lua_state, event.first.c_str());
        lua_createtable(lua_state, 0, 0);
        for (auto const& hook : event.second) {
            lua_pushstring(lua_state, hook.first.c_str());
            lua_rawgeti(lua_state, LUA_REGISTRYINDEX, hook.second);
            lua_settable(lua_state, -3);
        }
        lua_settable(lua_state, -3);
    }

    return 1;
}

int CLuaBase::lua$hook_remove()
{
    std::string event_name = lua_tostring(lua_state, 1);
    std::string identifier = lua_tostring(lua_state, 2);

    if (!registered_hooks.contains(event_name))
        return 0;

    if (!registered_hooks[event_name].contains(identifier))
        return 0;

    luaL_unref(lua_state, LUA_REGISTRYINDEX, registered_hooks[event_name][identifier]);
    registered_hooks[event_name].erase(identifier);
    return 0;
}

int CLuaBase::lua$hook_run()
{
    std::string event_name = lua_tostring(lua_state, 1);
    int number_of_arguments = lua_gettop(lua_state) - 1;
    int stack_top_without_args = lua_gettop(lua_state);

    lua_pushcfunction(lua_state, CLuaBase::lua$hook_call$entry);
    lua_pushstring(lua_state, event_name.c_str());
    // FIXME: Implement gamemodes.
    lua_pushnil(lua_state);
    for (int i = 1; i <= number_of_arguments; i++)
        lua_pushvalue(lua_state, 1 + i);
    lua_call(lua_state, 2 + number_of_arguments, LUA_MULTRET);
    return lua_gettop(lua_state) - stack_top_without_args;
}
