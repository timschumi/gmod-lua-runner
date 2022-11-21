#include <CLuaBase.h>
#include <GarrysMod/Lua/Interface.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <unistd.h>

int main(int argc, char const** argv)
{
    std::filesystem::path original_directory = std::filesystem::current_path();
    std::filesystem::path binary_directory = std::filesystem::absolute(argv[0]).parent_path();

    std::filesystem::path base_directory = getenv("GMOD_DIR") ?: binary_directory;
    if (!base_directory.is_absolute())
        base_directory = std::filesystem::absolute(base_directory);

    if (chdir(base_directory.string().c_str()) < 0) {
        fprintf(stderr, "Failed to change directory to '%s': %s\n", base_directory.string().c_str(), strerror(errno));
        return 1;
    }

    if (argc <= 1) {
        fprintf(stderr, "error: Need a script to run.\n");
        return 1;
    }

    std::filesystem::path script_path = argv[1];

    if (!script_path.is_absolute())
        script_path = original_directory / script_path;

    CLuaBase lua_base;

    lua_base.PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    lua_base.CreateTable();

    lua_base.PushNumber(0);
    lua_base.SetField(-2, "return_value");

    lua_base.PushString(base_directory.string().c_str(), 0);
    lua_base.SetField(-2, "root");

    lua_base.SetField(-2, "runner");
    lua_base.Pop(1);

    if (std::filesystem::exists("garrysmod/lua/autorun")) {
        lua_base.PushCFunction(CLuaBase::print_error_with_stack_trace);
        for (auto const& entry : std::filesystem::directory_iterator("garrysmod/lua/autorun")) {
            if (lua_base.load_file(entry.path().string().c_str()) != 0) {
                fprintf(stderr, "%s\n", lua_base.CheckString(-1));
                return 1;
            }
            if (lua_base.PCall(0, 0, -2) != 0)
                return 1;
        }
        lua_base.Pop(1);
    }

    lua_base.PushCFunction(CLuaBase::print_error_with_stack_trace);
    auto top_before_script = lua_base.Top();
    if (lua_base.load_file(script_path.string().c_str()) != 0) {
        fprintf(stderr, "%s\n", lua_base.CheckString(-1));
        return 1;
    }
    if (lua_base.PCall(0, 0, -2) != 0)
        return 1;

    bool has_return_values = lua_base.Top() > top_before_script;
    int return_value = has_return_values ? static_cast<int>(lua_base.CheckNumber(-1)) : 0;

    lua_base.is_active_override = [](GarrysMod::Lua::ILuaBase* LUA) -> std::optional<bool> {
        LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->GetField(-1, "runner");
        LUA->GetField(-1, "active");
        bool is_present = LUA->GetType(-1) != GarrysMod::Lua::Type::Nil;
        bool is_active = LUA->GetBool(-1);
        LUA->Pop(3);

        if (!is_present)
            return {};

        return is_active;
    };

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
