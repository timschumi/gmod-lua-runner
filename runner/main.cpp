#include <CLuaBase.h>
#include <GarrysMod/Lua/Interface.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <unistd.h>

int main(int argc, char const** argv)
{
    char* directory_from_env = getenv("GMOD_DIR");
    std::string original_directory = std::filesystem::current_path().generic_string();

    if (directory_from_env) {
        if (chdir(directory_from_env) < 0) {
            fprintf(stderr, "Failed to change directory to '%s': %s\n", directory_from_env, strerror(errno));
            return 1;
        }
    }

    if (argc <= 1) {
        fprintf(stderr, "error: Need a script to run.\n");
        return 1;
    }

    std::string script_path = argv[1];

    if (!script_path.starts_with('/'))
        script_path = original_directory + "/" + script_path;

    CLuaBase lua_base;

    switch (lua_base.load_and_run_file(script_path.c_str())) {
    case CLuaBase::Success: {
        int return_value = static_cast<int>(lua_base.CheckNumber(-1));
        lua_base.run_event_loop();
        return return_value;
    }
    case CLuaBase::SyntaxError:
        fprintf(stderr, "Syntax Error: %s\n", lua_base.CheckString(-1));
        return 1;
    case CLuaBase::MemoryAllocationError:
        fprintf(stderr, "Memory Allocation Error: %s\n", lua_base.CheckString(-1));
        return 1;
    case CLuaBase::FileError:
        fprintf(stderr, "File Error: %s\n", lua_base.CheckString(-1));
        return 1;
    case CLuaBase::RuntimeError:
        fprintf(stderr, "Runtime Error: %s\n", lua_base.CheckString(-1));
        return 1;
    case CLuaBase::ErrorError:
        fprintf(stderr, "Error while handling Errors: %s\n", lua_base.CheckString(-1));
        return 1;
    }

    return 0;
}
