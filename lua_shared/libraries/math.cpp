#include "CLuaBase.h"
#include <cassert>
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/math.min
int CLuaBase::lua$math_min()
{
    int number_of_values = lua_gettop(lua_state);

    double result = luaL_checknumber(lua_state, 1);

    for (int i = 2; i <= number_of_values; i++) {
        double new_value = luaL_checknumber(lua_state, i);

        if (new_value < result)
            result = new_value;
    }

    lua_pushnumber(lua_state, result);
    return 1;
}
