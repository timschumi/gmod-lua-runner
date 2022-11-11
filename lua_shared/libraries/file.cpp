#include "CLuaBase.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <lua.hpp>

// https://wiki.facepunch.com/gmod/File_Search_Paths
static std::optional<std::list<std::string>> file_search_path_to_list(std::string const& path)
{
    if (path == "LUA")
        return std::list<std::string> { "garrysmod/lua" };
    else
        return {};
}

static std::ios_base::openmode mode_to_ios_flags(std::string const& mode)
{
    if (mode == "r")
        return std::ios_base::in;
    if (mode == "w")
        return std::ios_base::out;
    if (mode == "a")
        return std::ios_base::out | std::ios_base::app;
    if (mode == "rb")
        return std::ios_base::in | std::ios_base::binary;
    if (mode == "wb")
        return std::ios_base::out | std::ios_base::binary;
    if (mode == "ab")
        return std::ios_base::out | std::ios_base::app | std::ios_base::binary;

    assert(false);
}

// https://wiki.facepunch.com/gmod/file.Find
int CLuaBase::lua$file_Find()
{
    std::string name = lua_tostring(lua_state, 1);
    std::string path = lua_tostring(lua_state, 2);
    // FIXME: Sorting is currently ignored.
    std::string sorting = lua_gettop(lua_state) >= 3 ? lua_tostring(lua_state, 3) : "nameasc";

    auto paths_to_search = file_search_path_to_list(path);

    if (!paths_to_search)
        return luaL_error(lua_state, "Unknown or unimplemented path for finding files: '%s'", path.c_str());

    lua_createtable(lua_state, 0, 0);
    lua_createtable(lua_state, 0, 0);

    for (auto const& directory_path : paths_to_search.value()) {
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

// https://wiki.facepunch.com/gmod/file.Open
int CLuaBase::lua$file_Open()
{
    // FIXME: Sanitize path against directory traversal.
    std::string file_name = lua_tostring(lua_state, 1);
    std::string file_mode = lua_tostring(lua_state, 2);
    std::string game_path = lua_tostring(lua_state, 3);

    auto paths_to_search = file_search_path_to_list(game_path);

    if (!paths_to_search)
        return luaL_error(lua_state, "Unknown or unimplemented path for finding files: '%s'", game_path.c_str());

    for (auto const& directory_path : paths_to_search.value()) {
        std::string full_path = directory_path + "/" + file_name;

        // This is manually allocated because we will be storing it as userdata.
        auto* file = new std::fstream(full_path.c_str(), mode_to_ios_flags(file_mode));

        if (file->fail()) {
            delete file;
            continue;
        }

        auto userdata = static_cast<std::fstream**>(lua_newuserdata(lua_state, sizeof(void*)));
        *userdata = file;
        luaL_setmetatable(lua_state, "File");

        return 1;
    }

    lua_pushnil(lua_state);
    return 1;
}
