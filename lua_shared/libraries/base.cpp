#include "CLuaBase.h"
#include <dlfcn.h>
#include <lua.hpp>

int CLuaBase::lua$cur_time()
{
    lua_pushnumber(lua_state, uptime);
    return 1;
}

int CLuaBase::lua$print()
{
    int nargs = lua_gettop(lua_state);

    for (int i = 1; i <= nargs; i++) {
        int type = lua_type(lua_state, i);
        switch (type) {
        case LUA_TNIL:
            printf("nil");
            break;
        case LUA_TBOOLEAN:
            printf(lua_toboolean(lua_state, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            printf("%g", lua_tonumber(lua_state, i));
            break;
        case LUA_TSTRING:
            printf("%s", lua_tostring(lua_state, i));
            break;
        case LUA_TTHREAD:
            printf("<thread>");
            break;
        default:
            printf("<unknown type %d>", type);
            break;
        }

        if (i < nargs)
            printf("\t");
    }

    printf("\n");
    return 0;
}

int CLuaBase::lua$print_table()
{
    // Note: `done` is ignored.
    // FIXME: Handle non-string keys and values.
    int indent = lua_tonumber(lua_state, 2);

    lua_pushnil(lua_state);
    while (lua_next(lua_state, 1) != 0) {
        for (int i = 0; i < indent; i++)
            printf("\t");

        auto key = lua_tostring(lua_state, -2);

        if (lua_type(lua_state, -1) == LUA_TTABLE) {
            printf("%s:\n", key);
            lua_pushcfunction(lua_state, CLuaBase::lua$print_table$entry);
            lua_pushvalue(lua_state, -2);
            lua_pushnumber(lua_state, indent + 2);
            lua_call(lua_state, 2, 0);
        } else {
            auto value = lua_tostring(lua_state, -1);
            printf("%s\t=\t%s\n", key, value);
        }

        lua_pop(lua_state, 1);
    }
    return 0;
}

int CLuaBase::lua$require()
{
    char full_name[32];

    char const* name = lua_tostring(lua_state, -1);
    snprintf(full_name, sizeof(full_name), "gmsv_%s_" GMOD_MODULE_ARCH ".dll", name);

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
