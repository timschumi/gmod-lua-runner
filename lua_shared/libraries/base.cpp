#include "CLuaBase.h"
#include <dlfcn.h>
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/Global.CurTime
int CLuaBase::lua$CurTime()
{
    lua_pushnumber(lua_state, uptime);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.getfenv
int CLuaBase::lua$getfenv()
{
    if (lua_type(lua_state, 1) == LUA_TNUMBER) {
        lua_Debug ar {};
        lua_getstack(lua_state, lua_tonumber(lua_state, 1), &ar);
        lua_getinfo(lua_state, "f", &ar);
        lua_replace(lua_state, 1);
    }

    lua_getfenv(lua_state, 1);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.include
int CLuaBase::lua$include()
{
    std::string path = (std::string) "garrysmod/lua/" + lua_tostring(lua_state, 1);
    int initial_top = lua_gettop(lua_state);

    if (luaL_loadfile(lua_state, path.c_str()) != LUA_OK)
        return lua_error(lua_state);

    lua_call(lua_state, 0, LUA_MULTRET);

    return lua_gettop(lua_state) - initial_top;
}

static int ipairs_iterator(lua_State* lua_state)
{
    lua_pushnumber(lua_state, lua_tonumber(lua_state, 2) + 1);
    lua_replace(lua_state, 2);

    if (lua_tonumber(lua_state, 2) > lua_objlen(lua_state, 1))
        return 0;

    lua_pushvalue(lua_state, 2);
    lua_pushvalue(lua_state, 2);
    lua_gettable(lua_state, 1);

    return 2;
}

// https://wiki.facepunch.com/gmod/Global.ipairs
int CLuaBase::lua$ipairs()
{
    lua_pushcfunction(lua_state, ipairs_iterator);
    lua_pushvalue(lua_state, 1);
    lua_pushnumber(lua_state, 0);
    return 3;
}

// https://wiki.facepunch.com/gmod/Global.istable
int CLuaBase::lua$istable()
{
    lua_pushboolean(lua_state, lua_type(lua_state, 1) == LUA_TTABLE);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.MsgC
int CLuaBase::lua$MsgC()
{
    int number_of_arguments = lua_gettop(lua_state);

    for (int i = 1; i <= number_of_arguments; i++) {
        // FIXME: Support colors.
        if (lua_getmetatable(lua_state, i) != 0) {
            lua_getfield(lua_state, LUA_REGISTRYINDEX, "Color");
            bool is_color = lua_rawequal(lua_state, -1, -2) == 1;
            lua_pop(lua_state, 2);
            if (is_color)
                continue;
        }

        lua_pushcfunction(lua_state, lua$tostring$entry);
        lua_pushvalue(lua_state, i);
        lua_call(lua_state, 1, 1);
        printf("%s", lua_tostring(lua_state, -1));
        lua_pop(lua_state, 1);
    }
    return 0;
}

// https://wiki.facepunch.com/gmod/Global.next
int CLuaBase::lua$next()
{
    if (lua_gettop(lua_state) >= 2)
        lua_pushvalue(lua_state, 2);
    else
        lua_pushnil(lua_state);
    if (lua_next(lua_state, 1) == 0) {
        lua_pushnil(lua_state);
        lua_pushnil(lua_state);
    }
    return 2;
}

// https://wiki.facepunch.com/gmod/Global.pairs
int CLuaBase::lua$pairs()
{
    lua_pushcfunction(lua_state, lua$next$entry);
    lua_pushvalue(lua_state, 1);
    lua_pushnil(lua_state);
    return 3;
}

// https://wiki.facepunch.com/gmod/Global.print
int CLuaBase::lua$print()
{
    int nargs = lua_gettop(lua_state);

    for (int i = 1; i <= nargs; i++) {
        lua_pushcfunction(lua_state, lua$tostring$entry);
        lua_pushvalue(lua_state, i);
        lua_call(lua_state, 1, 1);
        printf("%s", lua_tostring(lua_state, -1));
        lua_pop(lua_state, 1);

        if (i < nargs)
            printf("\t");
    }

    printf("\n");
    return 0;
}

// https://wiki.facepunch.com/gmod/Global.PrintTable
int CLuaBase::lua$PrintTable()
{
    // Note: `done` is ignored.
    int indent = lua_tonumber(lua_state, 2);

    lua_pushnil(lua_state);
    while (lua_next(lua_state, 1) != 0) {
        for (int i = 0; i < indent; i++)
            printf("\t");

        lua_pushcfunction(lua_state, lua$tostring$entry);
        lua_pushvalue(lua_state, -3);
        lua_call(lua_state, 1, 1);
        char const* key = lua_tostring(lua_state, -1);
        lua_pop(lua_state, 1);

        if (lua_type(lua_state, -1) == LUA_TTABLE) {
            printf("%s:\n", key);
            lua_pushcfunction(lua_state, CLuaBase::lua$PrintTable$entry);
            lua_pushvalue(lua_state, -2);
            lua_pushnumber(lua_state, indent + 2);
            lua_call(lua_state, 2, 0);
        } else {
            lua_pushcfunction(lua_state, lua$tostring$entry);
            lua_pushvalue(lua_state, -2);
            lua_call(lua_state, 1, 1);
            char const* value = lua_tostring(lua_state, -1);
            lua_pop(lua_state, 1);
            printf("%s\t=\t%s\n", key, value);
        }

        lua_pop(lua_state, 1);
    }
    return 0;
}

// https://wiki.facepunch.com/gmod/Global.require
int CLuaBase::lua$require()
{
    char const* format = "garrysmod/lua/bin/gmsv_%s_" GMOD_MODULE_ARCH ".dll";
    char const* module_name = lua_tostring(lua_state, -1);

    size_t formatted_name_length = snprintf(nullptr, 0, format, module_name);
    char full_name[formatted_name_length + 1];
    snprintf(full_name, sizeof(full_name), format, module_name);

    void* library_handle = dlopen(full_name, RTLD_LAZY);
    if (!library_handle) {
        return luaL_error(lua_state, "dlopen failed: %s", dlerror());
    }

    auto library_init_function = reinterpret_cast<lua_CFunction>(dlsym(library_handle, "gmod13_open"));
    if (!library_init_function) {
        return luaL_error(lua_state, "dlsym failed: %s", dlerror());
    }

    lua_pushcfunction(lua_state, library_init_function);
    lua_call(lua_state, 0, 0);

    loaded_module_handles.push_back(library_handle);

    return 0;
}

// https://wiki.facepunch.com/gmod/Global.setfenv
int CLuaBase::lua$setfenv()
{
    bool function_passed = true;
    if (lua_type(lua_state, 1) == LUA_TNUMBER) {
        function_passed = false;
        lua_Debug ar {};
        lua_getstack(lua_state, lua_tonumber(lua_state, 1), &ar);
        lua_getinfo(lua_state, "f", &ar);
        lua_replace(lua_state, 1);
    }

    lua_pushvalue(lua_state, 2);
    lua_setfenv(lua_state, 1);

    if (function_passed)
        lua_pushvalue(lua_state, 1);
    else
        lua_pushnil(lua_state);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.setmetatable
int CLuaBase::lua$setmetatable()
{
    lua_pushvalue(lua_state, 2);
    lua_setmetatable(lua_state, 1);

    lua_pushvalue(lua_state, 1);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.tostring
int CLuaBase::lua$tostring()
{
    // FIXME: This should use __tostring where applicable.
    auto format_value = [&](char* buffer, size_t buffer_size) {
        int type = lua_type(lua_state, 1);
        switch (type) {
        case LUA_TNIL:
            return snprintf(buffer, buffer_size, "nil");
        case LUA_TBOOLEAN:
            return snprintf(buffer, buffer_size, lua_toboolean(lua_state, 1) ? "true" : "false");
        case LUA_TNUMBER:
            return snprintf(buffer, buffer_size, "%g", lua_tonumber(lua_state, 1));
        case LUA_TSTRING:
            return snprintf(buffer, buffer_size, "%s", lua_tostring(lua_state, 1));
        case LUA_TUSERDATA: {
            void* userdata = lua_touserdata(lua_state, 1);
            lua_getmetatable(lua_state, 1);
            lua_pushstring(lua_state, "__name");
            lua_rawget(lua_state, -2);
            char const* name = lua_tostring(lua_state, -1);
            lua_pop(lua_state, 2);
            return snprintf(buffer, buffer_size, "<%p:%s>", userdata, name);
        }
        case LUA_TTHREAD:
            return snprintf(buffer, buffer_size, "<thread>");
        default:
            return snprintf(buffer, buffer_size, "<unknown type %d>", type);
        }
    };

    int required_buffer_size = format_value(nullptr, 0) + 1;
    char buffer[required_buffer_size];
    format_value(buffer, required_buffer_size);
    lua_pushstring(lua_state, buffer);

    return 1;
}

// https://wiki.facepunch.com/gmod/Global.xpcall
int CLuaBase::lua$xpcall()
{
    int top_of_stack = lua_gettop(lua_state);
    int number_of_varargs = top_of_stack - 2;

    lua_pushvalue(lua_state, 1);
    for (int i = 1; i <= number_of_varargs; i++)
        lua_pushvalue(lua_state, 2 + i);
    int ret = lua_pcall(lua_state, number_of_varargs, LUA_MULTRET, 2);

    if (ret == 0) {
        int number_of_return_values = lua_gettop(lua_state) - top_of_stack + 1;
        lua_pushboolean(lua_state, true);
        lua_insert(lua_state, -number_of_return_values);
        return number_of_return_values;
    }

    lua_pushboolean(lua_state, false);
    lua_insert(lua_state, -2);
    return 2;
}

void CLuaBase::unload_modules()
{
    for (auto handle : loaded_module_handles) {
        auto library_fini_function = reinterpret_cast<lua_CFunction>(dlsym(handle, "gmod13_close"));
        if (!library_fini_function) {
            fprintf(stderr, "dlsym failed: %s\n", dlerror());
            continue;
        }

        lua_pushcfunction(lua_state, library_fini_function);
        lua_call(lua_state, 0, 0);

        dlclose(handle);
    }

    loaded_module_handles.clear();
}
