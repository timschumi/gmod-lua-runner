#include <CLuaBase.h>
#include <GarrysMod/Lua/Interface.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <unistd.h>

int main(int argc, char const** argv)
{
    std::string original_directory = std::filesystem::current_path().generic_string();

    std::filesystem::path directory_from_env = std::filesystem::absolute(getenv("GMOD_DIR") ?: std::filesystem::path(argv[0]).parent_path());

    if (chdir(directory_from_env.c_str()) < 0) {
        fprintf(stderr, "Failed to change directory to '%s': %s\n", directory_from_env.c_str(), strerror(errno));
        return 1;
    }

    if (argc <= 1) {
        fprintf(stderr, "error: Need a script to run.\n");
        return 1;
    }

    std::string script_path = argv[1];

    if (!script_path.starts_with('/'))
        script_path = original_directory + "/" + script_path;

    CLuaBase lua_base;

    lua_base.PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    lua_base.CreateTable();

    lua_base.PushNumber(0);
    lua_base.SetField(-2, "return_value");

    lua_base.PushString(directory_from_env.c_str(), 0);
    lua_base.SetField(-2, "root");

    lua_base.SetField(-2, "runner");
    lua_base.Pop(1);

    if (std::filesystem::exists("garrysmod/lua/autorun")) {
        for (auto const& entry : std::filesystem::directory_iterator("garrysmod/lua/autorun")) {
            if (lua_base.load_and_run_file_or_show_error(entry.path().c_str()) != CLuaBase::Success)
                return 1;
        }
    }

    auto top_before_script = lua_base.Top();
    auto result = lua_base.load_and_run_file_or_show_error(script_path.c_str());

    if (result != CLuaBase::Success)
        return 1;

    bool has_return_values = lua_base.Top() > top_before_script;
    int return_value = has_return_values ? static_cast<int>(lua_base.CheckNumber(-1)) : 0;

    lua_base.run_event_loop();

    if (!has_return_values) {
        lua_base.PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        lua_base.GetField(-1, "runner");
        lua_base.GetField(-1, "return_value");
        return_value = lua_base.CheckNumber(-1);
        lua_base.Pop(3);
    }

    return return_value;
}
