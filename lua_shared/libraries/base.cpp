#include "CLuaBase.h"
#include <lua.hpp>
#include <dlfcn.h>

int CLuaBase::lua$print()
{
    int nargs = lua_gettop(lua_state);

    for (int i = 1; i <= nargs; i++) {
        char const* string = lua_tostring(lua_state, i);
        printf("%s%s", string, i < nargs ? "\t" : "");
    }

    printf("\n");
    return 0;
}

int CLuaBase::lua$require()
{
    char full_name[32];

    char const* name = lua_tostring(lua_state, -1);
    snprintf(full_name, sizeof(full_name), "gmsv_%s_linux64.dll", name);

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
    }

    loaded_module_handles.clear();
}
