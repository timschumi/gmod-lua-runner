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

    int return_value = lua_base.Top() > top_before_script ? static_cast<int>(lua_base.CheckNumber(-1)) : 0;
    lua_base.run_event_loop();
    return return_value;
}
