#include "CLuaBase.h"
#include <cassert>
#include <filesystem>
#include <lua.hpp>

#ifdef __linux__
#    include <dlfcn.h>
#endif

#ifdef _WIN32
#    include <windows.h>
#endif

// https://wiki.facepunch.com/gmod/Global.Color
int CLuaBase::lua$meta$Color_eq(lua_State* lua_state)
{
#define COMPARE_INDEX(name)                     \
    lua_pushstring(lua_state, name);            \
    lua_gettable(lua_state, 1);                 \
    lua_pushstring(lua_state, name);            \
    lua_gettable(lua_state, 2);                 \
    if (lua_rawequal(lua_state, -1, -2) == 0) { \
        lua_pushboolean(lua_state, false);      \
        return 1;                               \
    }
    COMPARE_INDEX("r")
    COMPARE_INDEX("g")
    COMPARE_INDEX("b")
    COMPARE_INDEX("a")

    lua_pushboolean(lua_state, true);
    return 1;
#undef COMPARE_INDEX
}

// https://wiki.facepunch.com/gmod/Global.Color
int CLuaBase::lua$Color(lua_State* lua_state)
{
    int number_of_arguments = lua_gettop(lua_state);

    lua_createtable(lua_state, 0, 4);

    lua_pushstring(lua_state, "r");
    lua_pushvalue(lua_state, 1);
    lua_settable(lua_state, -3);

    lua_pushstring(lua_state, "g");
    lua_pushvalue(lua_state, 2);
    lua_settable(lua_state, -3);

    lua_pushstring(lua_state, "b");
    lua_pushvalue(lua_state, 3);
    lua_settable(lua_state, -3);

    lua_pushstring(lua_state, "a");
    if (number_of_arguments >= 4)
        lua_pushvalue(lua_state, 4);
    else
        lua_pushnumber(lua_state, 255);
    lua_settable(lua_state, -3);

    luaL_setmetatable(lua_state, "Color");

    return 1;
}

// https://wiki.facepunch.com/gmod/Global.CompileString
int CLuaBase::lua$CompileString(lua_State* lua_state)
{
    int number_of_arguments = lua_gettop(lua_state);
    size_t code_len = 0;
    char const* code = luaL_checklstring(lua_state, 1, &code_len);
    std::string identifier = luaL_checkstring(lua_state, 2);
    bool handleError = true;
    if (number_of_arguments >= 3) {
        luaL_argcheck(lua_state, lua_isboolean(lua_state, 3), 3, "Expected boolean");
        handleError = lua_toboolean(lua_state, 3);
    }

    // Make sure that we are treating the source identifier as-is.
    identifier.insert(0, 1, '=');

    int load_result = luaL_loadbuffer(lua_state, code, code_len, identifier.c_str());
    if (load_result == LUA_OK || !handleError)
        return 1;

    return lua_error(lua_state);
}

// https://wiki.facepunch.com/gmod/ConVar:GetBool
int CLuaBase::lua$meta$ConVar_GetBool(lua_State* lua_state)
{
    auto convar = static_cast<ConVar**>(luaL_checkudata(lua_state, 1, "ConVar"));

    // If the value is numeric and not 0, the result will be true. Otherwise, the result will be false.
    lua_pushboolean(lua_state, strtod((*convar)->value.c_str(), nullptr) != 0);

    return 1;
}

// https://wiki.facepunch.com/gmod/ConVar:SetBool
int CLuaBase::lua$meta$ConVar_SetBool(lua_State* lua_state)
{
    auto convar = static_cast<ConVar**>(luaL_checkudata(lua_state, 1, "ConVar"));
    auto value = lua_toboolean(lua_state, 2);

    (*convar)->value = value ? "1" : "0";

    return 0;
}

// https://wiki.facepunch.com/gmod/Global.CreateConVar
int CLuaBase::lua$CreateConVar(lua_State* lua_state)
{
    int number_of_arguments = lua_gettop(lua_state);
    std::string name = luaL_checkstring(lua_state, 1);
    std::string value = luaL_checkstring(lua_state, 2);
    long flags = (number_of_arguments >= 3) ? static_cast<long>(luaL_checknumber(lua_state, 3)) : FCVAR_NONE;
    auto helptext = (number_of_arguments >= 4) ? std::optional<std::string> { luaL_checkstring(lua_state, 4) } : std::nullopt;
    auto min = (number_of_arguments >= 5) ? std::optional<double> { luaL_checknumber(lua_state, 5) } : std::nullopt;
    auto max = (number_of_arguments >= 6) ? std::optional<double> { luaL_checknumber(lua_state, 6) } : std::nullopt;

    if (!convars.contains(name)) {
        convars[name] = { value, flags, helptext, min, max };
    }

    ConVar* convar = &convars[name];

    auto userdata = static_cast<ConVar**>(lua_newuserdata(lua_state, sizeof(void*)));
    *userdata = convar;
    luaL_setmetatable(lua_state, "ConVar");

    return 1;
}

// https://wiki.facepunch.com/gmod/Global.CurTime
int CLuaBase::lua$CurTime(lua_State* lua_state)
{
    lua_pushnumber(lua_state, uptime);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.ErrorNoHaltWithStack
int CLuaBase::lua$ErrorNoHaltWithStack(lua_State* lua_state)
{
    int nargs = lua_gettop(lua_state);

    for (int i = 1; i <= nargs; i++) {
        lua_pushcfunction(lua_state, lua$tostring$entry);
        lua_pushvalue(lua_state, i);
        lua_call(lua_state, 1, 1);
        fprintf(stderr, "%s", lua_tostring(lua_state, -1));
        lua_pop(lua_state, 1);
    }

    fprintf(stderr, "\n");

    print_stack_trace(lua_state);

    return 0;
}

// https://wiki.facepunch.com/gmod/Global.GetConVar
int CLuaBase::lua$GetConVar(lua_State* lua_state)
{
    std::string name = luaL_checkstring(lua_state, 1);

    if (!convars.contains(name)) {
        lua_pushnil(lua_state);
        return 1;
    }

    ConVar* convar = &convars[name];

    auto userdata = static_cast<ConVar**>(lua_newuserdata(lua_state, sizeof(void*)));
    *userdata = convar;
    luaL_setmetatable(lua_state, "ConVar");

    return 1;
}

// https://wiki.facepunch.com/gmod/Global.include
int CLuaBase::lua$include(lua_State* lua_state)
{
    std::string path = luaL_checkstring(lua_state, 1);
    int initial_top = lua_gettop(lua_state);

    std::filesystem::path built_path;

    // TODO: Check behavior on include calls from [C].
    lua_Debug ar {};
    if (lua_getstack(lua_state, 1, &ar) == 0)
        goto include_skip_current;
    if (lua_getinfo(lua_state, "S", &ar) == 0)
        goto include_skip_current;
    if (ar.source[0] != '@')
        goto include_skip_current;

    // TODO: Check preference between relative-to-current and relative-to-root. For now we prefer relative-to-current.
    built_path = std::filesystem::path { ar.source + 1 }.parent_path() / path;
    if (std::filesystem::exists(built_path))
        goto include_load;
include_skip_current:
    built_path = this->base_directory / "garrysmod/lua" / path;

include_load:
    if (luaL_loadfile(lua_state, built_path.string().c_str()) != LUA_OK)
        return lua_error(lua_state);

    lua_call(lua_state, 0, LUA_MULTRET);

    return lua_gettop(lua_state) - initial_top;
}

// https://wiki.facepunch.com/gmod/Global.isfunction
int CLuaBase::lua$isfunction(lua_State* lua_state)
{
    lua_pushboolean(lua_state, lua_type(lua_state, 1) == LUA_TFUNCTION);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.isstring
int CLuaBase::lua$isstring(lua_State* lua_state)
{
    lua_pushboolean(lua_state, lua_type(lua_state, 1) == LUA_TSTRING);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.istable
int CLuaBase::lua$istable(lua_State* lua_state)
{
    lua_pushboolean(lua_state, lua_type(lua_state, 1) == LUA_TTABLE);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.MsgC
int CLuaBase::lua$MsgC(lua_State* lua_state)
{
    int number_of_arguments = lua_gettop(lua_state);

    for (int i = 1; i <= number_of_arguments; i++) {
        if (lua_getmetatable(lua_state, i) != 0) {
            lua_getfield(lua_state, LUA_REGISTRYINDEX, "Color");
            bool is_color = lua_rawequal(lua_state, -1, -2) == 1;
            lua_pop(lua_state, 2);

            if (is_color) {
                lua_getfield(lua_state, i, "r");
                int r = lua_tonumber(lua_state, -1);
                lua_getfield(lua_state, i, "g");
                int g = lua_tonumber(lua_state, -1);
                lua_getfield(lua_state, i, "b");
                int b = lua_tonumber(lua_state, -1);
                lua_pop(lua_state, 3);

                printf("\x1b[38;2;%d;%d;%dm", r, g, b);
                continue;
            }
        }

        lua_pushcfunction(lua_state, lua$tostring$entry);
        lua_pushvalue(lua_state, i);
        lua_call(lua_state, 1, 1);
        printf("%s", lua_tostring(lua_state, -1));
        lua_pop(lua_state, 1);
    }
    printf("\x1b[39m");
    return 0;
}

// https://wiki.facepunch.com/gmod/Global.PrintTable
int CLuaBase::lua$PrintTable(lua_State* lua_state)
{
    // Note: `done` is ignored.
    luaL_argcheck(lua_state, lua_istable(lua_state, 1), 1, "Expected table");
    int indent = lua_gettop(lua_state) >= 2 ? luaL_checknumber(lua_state, 2) : 0;

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

// https://wiki.facepunch.com/gmod/Global.ProtectedCall
int CLuaBase::lua$ProtectedCall(lua_State* lua_state)
{
    // FIXME: This should still print and handle the error in the error case.

    // One function and all arguments, they can stay as-is.
    int number_of_arguments = lua_gettop(lua_state);
    luaL_argcheck(lua_state, lua_isfunction(lua_state, 1), 1, "Expected function");

    int call_result = lua_pcall(lua_state, number_of_arguments - 1, 0, 0);
    lua_pushboolean(lua_state, call_result == LUA_OK);
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.require
int CLuaBase::lua$require(lua_State* lua_state)
{
    char const* format = "%s/garrysmod/lua/bin/gmsv_%s_" GMOD_MODULE_ARCH ".dll";
    std::string module_name = luaL_checkstring(lua_state, 1);

    if (loaded_module_handles.contains(module_name))
        return 0;

    size_t formatted_name_length = snprintf(nullptr, 0, format, this->base_directory.string().c_str(), module_name.c_str());
    char full_name[formatted_name_length + 1];
    snprintf(full_name, sizeof(full_name), format, this->base_directory.string().c_str(), module_name.c_str());

    if (!std::filesystem::exists(std::filesystem::path { full_name, full_name + formatted_name_length })) {
        // If the module doesn't exist then delegate to the Lua module system.
        // TODO: Check which error message is printed if neither exists.
        assert(lua$require$original != -1);
        lua_rawgeti(lua_state, LUA_REGISTRYINDEX, lua$require$original);
        lua_pushvalue(lua_state, 1);
        lua_call(lua_state, 1, 0);
        return 0;
    }

#if defined(__linux__)
    void* library_handle = dlopen(full_name, RTLD_LAZY);
    if (!library_handle) {
        return luaL_error(lua_state, "dlopen failed: %s", dlerror());
    }

    auto library_init_function = reinterpret_cast<lua_CFunction>(dlsym(library_handle, "gmod13_open"));
    if (!library_init_function) {
        return luaL_error(lua_state, "dlsym failed: %s", dlerror());
    }
#elif defined(_WIN32)
    HINSTANCE library_handle = LoadLibrary(full_name);
    if (!library_handle)
        return luaL_error(lua_state, "LoadLibrary failed");

    auto library_init_function = reinterpret_cast<lua_CFunction>(GetProcAddress(library_handle, "gmod13_open"));
    if (!library_init_function)
        return luaL_error(lua_state, "Failed to find module initialization function");
#else
    assert(false);
#endif

    lua_pushcfunction(lua_state, library_init_function);
    lua_call(lua_state, 0, 0);

    loaded_module_handles[module_name] = library_handle;

    return 0;
}

// https://wiki.facepunch.com/gmod/Global.SysTime
int CLuaBase::lua$SysTime(lua_State* lua_state)
{
    std::chrono::duration<double> since_boot = std::chrono::system_clock::now() - boot_system_clock;
    lua_pushnumber(lua_state, since_boot.count());
    return 1;
}

// https://wiki.facepunch.com/gmod/Global.type
int CLuaBase::lua$type(lua_State* lua_state)
{
    // The builtin implementation of type() throws an error for this case.
    if (lua_gettop(lua_state) == 0) {
        lua_pushstring(lua_state, "no value");
        return 1;
    }

    assert(lua$type$original != -1);
    lua_rawgeti(lua_state, LUA_REGISTRYINDEX, lua$type$original);
    lua_pushvalue(lua_state, 1);
    lua_call(lua_state, 1, 1);

    return 1;
}

void CLuaBase::unload_modules()
{
    auto lua_state = luaR_current_thread(main_lua_state);

    // Nobody should ever run this from a non-main thread.
    assert(lua_state == main_lua_state);

    for (auto const& handle : loaded_module_handles) {
#if defined(__linux__)
        auto library_fini_function = reinterpret_cast<lua_CFunction>(dlsym(handle.second, "gmod13_close"));
        if (!library_fini_function) {
            fprintf(stderr, "dlsym failed: %s\n", dlerror());
            continue;
        }

        lua_pushcfunction(lua_state, library_fini_function);
        lua_call(lua_state, 0, 0);

        dlclose(handle.second);
#elif defined(_WIN32)
        auto library_fini_function = reinterpret_cast<lua_CFunction>(GetProcAddress(reinterpret_cast<HMODULE>(handle.second), "gmod13_close"));
        if (!library_fini_function) {
            fprintf(stderr, "Failed to find module deinitialization function\n");
            continue;
        }

        lua_pushcfunction(lua_state, library_fini_function);
        lua_call(lua_state, 0, 0);

        FreeLibrary(reinterpret_cast<HMODULE>(handle.second));
#else
        assert(false);
#endif
    }

    loaded_module_handles.clear();
}
