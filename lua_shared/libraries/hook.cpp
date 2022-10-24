#include "CLuaBase.h"
#include <lua.hpp>

int CLuaBase::lua$hook_add()
{
    char const* event_name = lua_tostring(lua_state, -3);
    char const* identifier = lua_tostring(lua_state, -2);
    lua_CFunction func = lua_tocfunction(lua_state, -1);

    // FIXME: Implement hooks.
    printf("FIXME: hook.Add('%s', '%s', %p) is not implemented.\n", event_name, identifier, func);

    return 0;
}
