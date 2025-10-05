#include "CLuaBase.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <lua.hpp>
#include <map>
#include <ranges>

struct FileHandle {
    std::string name;
    std::fstream stream;
};

// https://wiki.facepunch.com/gmod/File_Search_Paths
static std::optional<std::list<std::string>> file_search_path_to_list(std::string const& path)
{
    if (path == "LUA")
        return std::list<std::string> { "garrysmod/lua" };
    else if (path == "DATA")
        return std::list<std::string> { "garrysmod/data" };
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

// https://wiki.facepunch.com/gmod/File:Close
int CLuaBase::lua$meta$File_Close(lua_State* lua_state)
{
    auto** file = static_cast<FileHandle**>(luaL_checkudata(lua_state, 1, "File"));
    (*file)->stream.close();
    return 0;
}

// https://wiki.facepunch.com/gmod/File:Size
int CLuaBase::lua$meta$File_Size(lua_State* lua_state)
{
    auto** file = static_cast<FileHandle**>(luaL_checkudata(lua_state, 1, "File"));
    lua_pushnumber(lua_state, std::filesystem::file_size((*file)->name));
    return 1;
}

// https://wiki.facepunch.com/gmod/File:Read
int CLuaBase::lua$meta$File_Read(lua_State* lua_state)
{
    auto** file = static_cast<FileHandle**>(luaL_checkudata(lua_state, 1, "File"));
    std::string result;

    if (lua_gettop(lua_state) >= 2) {
        size_t length = luaL_checknumber(lua_state, 2);
        result.resize(length);
        (*file)->stream.read(&result[0], length);
    } else {
        std::stringstream buffer;
        buffer << (*file)->stream.rdbuf();
        result = buffer.str();
    }

    lua_pushlstring(lua_state, result.c_str(), result.size());
    return 1;
}

// https://wiki.facepunch.com/gmod/file.Exists
int CLuaBase::lua$file_Exists(lua_State* lua_state)
{
    // FIXME: Sanitize path against directory traversal.
    std::string file_name = luaL_checkstring(lua_state, 1);
    std::string game_path = luaL_checkstring(lua_state, 2);

    auto paths_to_search = file_search_path_to_list(game_path);

    if (!paths_to_search)
        return luaL_error(lua_state, "Unknown or unimplemented path for finding files: '%s'", game_path.c_str());

    for (auto const& directory_path : paths_to_search.value()) {
        std::filesystem::path full_path = this->base_directory / directory_path / file_name;

        if (std::filesystem::exists(full_path)) {
            lua_pushboolean(lua_state, true);
            return 1;
        }
    }

    lua_pushboolean(lua_state, false);
    return 1;
}

// https://wiki.facepunch.com/gmod/file.Find
int CLuaBase::lua$file_Find(lua_State* lua_state)
{
    std::string name = luaL_checkstring(lua_state, 1);
    std::string path = luaL_checkstring(lua_state, 2);
    std::string sorting = lua_gettop(lua_state) >= 3 ? luaL_checkstring(lua_state, 3) : "nameasc";

    // FIXME: Only nameasc sorting is implemented at the moment.
    if (sorting != "nameasc")
        return luaL_error(lua_state, "Unknown or unimplemented sorting scheme: '%s'", sorting.c_str());

    auto paths_to_search = file_search_path_to_list(path);

    if (!paths_to_search)
        return luaL_error(lua_state, "Unknown or unimplemented path for finding files: '%s'", path.c_str());

    lua_createtable(lua_state, 0, 0);
    lua_createtable(lua_state, 0, 0);

    for (auto const& directory_path : paths_to_search.value()) {
        auto full_name = this->base_directory.string() + "/" + directory_path;
        full_name.append("/");
        full_name.append(name);

        assert(full_name.ends_with("/*"));

        full_name = full_name.substr(0, full_name.size() - 2);

        if (!std::filesystem::exists(full_name))
            continue;

        // Rely on set order to sort paths in nameasc order, the choice of sorting order has been asserted above.
        // The paths should be unique, so using a set is fine.
        std::map<std::filesystem::path, std::filesystem::directory_entry> sorted_paths;
        for (auto const& entry : std::filesystem::directory_iterator { full_name })
            sorted_paths[entry.path()] = entry;

        for (auto const& dir_entry : sorted_paths | std::views::values) {
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
int CLuaBase::lua$file_Open(lua_State* lua_state)
{
    // FIXME: Sanitize path against directory traversal.
    std::string file_name = luaL_checkstring(lua_state, 1);
    std::string file_mode = luaL_checkstring(lua_state, 2);
    std::string game_path = luaL_checkstring(lua_state, 3);

    auto paths_to_search = file_search_path_to_list(game_path);

    if (!paths_to_search)
        return luaL_error(lua_state, "Unknown or unimplemented path for finding files: '%s'", game_path.c_str());

    for (auto const& directory_path : paths_to_search.value()) {
        std::string full_path = this->base_directory.string() + "/" + directory_path + "/" + file_name;

        std::fstream file(full_path.c_str(), mode_to_ios_flags(file_mode));

        if (file.fail())
            continue;

        // This is manually allocated because we will be storing it as userdata.
        auto* handle = new FileHandle;
        handle->name = std::move(full_path);
        handle->stream = std::move(file);

        auto userdata = static_cast<FileHandle**>(lua_newuserdata(lua_state, sizeof(void*)));
        *userdata = handle;
        luaL_setmetatable(lua_state, "File");

        return 1;
    }

    lua_pushnil(lua_state);
    return 1;
}

// https://wiki.facepunch.com/gmod/file.Read
int CLuaBase::lua$file_Read(lua_State* lua_state)
{
    // FIXME: Sanitize path against directory traversal.
    std::string file_name = luaL_checkstring(lua_state, 1);
    std::string game_path = lua_gettop(lua_state) >= 2 ? luaL_checkstring(lua_state, 2) : "DATA";

    auto paths_to_search = file_search_path_to_list(game_path);

    if (!paths_to_search)
        return luaL_error(lua_state, "Unknown or unimplemented path for finding files: '%s'", game_path.c_str());

    for (auto const& directory_path : paths_to_search.value()) {
        std::filesystem::path full_path = this->base_directory / directory_path / file_name;

        // TODO: std::ios_base::binary?
        std::fstream file(full_path.string().c_str(), std::ios_base::in);

        if (file.fail())
            continue;

        std::string result;
        std::stringstream buffer;
        buffer << file.rdbuf();
        result = buffer.str();

        lua_pushlstring(lua_state, result.c_str(), result.size());
        return 1;
    }

    lua_pushnil(lua_state);
    return 1;
}
