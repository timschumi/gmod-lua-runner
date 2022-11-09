#include "CLuaBase.h"
#include <cassert>
#include <filesystem>
#include <lua.hpp>

// Note: This is probably horribly inefficient.
static bool string_matches_wildcard(std::string_view string, std::string_view wildcard)
{
    if (string.length() == 0 && wildcard.length() == 0)
        return true;

    if (wildcard.length() == 0)
        return false;

    if (wildcard.starts_with('*'))
        return string_matches_wildcard(string.substr(1), wildcard.substr(1)) || string_matches_wildcard(string.substr(1), wildcard);

    if (string.length() == 0)
        return false;

    if (string[0] != wildcard[0])
        return false;

    return string_matches_wildcard(string.substr(1), wildcard.substr(1));
}

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
        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator { directory_path }) {
            // Remove search prefix from the file path.
            auto file_path = dir_entry.path().generic_string();
            assert(file_path.starts_with(directory_path + "/"));
            file_path = file_path.substr(directory_path.length() + 1);

            if (!string_matches_wildcard(file_path, name))
                continue;

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
