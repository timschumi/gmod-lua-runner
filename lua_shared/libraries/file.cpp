#include "CLuaBase.h"
#include <cassert>
#include <filesystem>
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/file.Find
int CLuaBase::lua$file_Find()
{
    std::string name = lua_tostring(lua_state, 1);
    std::string path = lua_tostring(lua_state, 2);
    // FIXME: Sorting is currently ignored.
    std::string sorting = lua_gettop(lua_state) >= 3 ? lua_tostring(lua_state, 3) : "nameasc";

    std::list<std::string> paths_to_search;

    if (path == "LUA") {
        paths_to_search.push_back("garrysmod/lua");
    } else {
        return luaL_error(lua_state, "Unknown or unimplemented path for finding files: '%s'", path.c_str());
    }

    lua_createtable(lua_state, 0, 0);
    lua_createtable(lua_state, 0, 0);

    for (auto const& directory_path : paths_to_search) {
        auto full_name = directory_path;
        full_name.append("/");
        full_name.append(name);

        assert(full_name.ends_with("/*"));

        full_name = full_name.substr(0, full_name.size() - 2);

        for (auto const& dir_entry : std::filesystem::directory_iterator { full_name }) {
            // Remove search prefix from the file path.
            auto file_path = dir_entry.path().generic_string();
            assert(file_path.starts_with(full_name + "/"));
            file_path = file_path.substr(full_name.length() + 1);

            if (dir_entry.is_directory())
                lua_pushvalue(lua_state, -1);
            else
                lua_pushvalue(lua_state, -2);

            lua_pushnumber(lua_state, lua_objlen(lua_state, -1) + 1);
            lua_pushstring(lua_state, file_path.c_str());
            lua_settable(lua_state, -3);

            lua_pop(lua_state, 1);
        }
    }

    return 2;
}
