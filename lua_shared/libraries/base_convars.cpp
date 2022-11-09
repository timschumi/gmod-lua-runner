#include "CLuaBase.h"
#include <lua.hpp>
#include <optional>

#define FCVAR_NONE 0

int CLuaBase::lua$meta$ConVar_GetBool()
{
    auto convar = static_cast<ConVar**>(lua_touserdata(lua_state, 1));

    // If the value is numeric and not 0, the result will be true. Otherwise, the result will be false.
    lua_pushboolean(lua_state, strtod((*convar)->value.c_str(), nullptr) != 0);

    return 1;
}

int CLuaBase::lua$CreateConVar()
{
    int number_of_arguments = lua_gettop(lua_state);
    std::string name = lua_tostring(lua_state, 1);
    std::string value = lua_tostring(lua_state, 2);
    long flags = (number_of_arguments >= 3) ? static_cast<long>(lua_tonumber(lua_state, 3)) : FCVAR_NONE;
    auto helptext = (number_of_arguments >= 4) ? std::optional<std::string> { lua_tostring(lua_state, 4) } : std::nullopt;
    auto min = (number_of_arguments >= 5) ? std::optional<double> { lua_tonumber(lua_state, 5) } : std::nullopt;
    auto max = (number_of_arguments >= 6) ? std::optional<double> { lua_tonumber(lua_state, 6) } : std::nullopt;

    if (!convars.contains(name)) {
        convars[name] = { value, flags, helptext, min, max };
    }

    ConVar* convar = &convars[name];

    auto userdata = static_cast<ConVar**>(lua_newuserdata(lua_state, sizeof(void*)));
    *userdata = convar;
    luaL_setmetatable(lua_state, "ConVar");

    return 1;
}
