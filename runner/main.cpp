#include <CLuaBase.h>
#include <GarrysMod/Lua/Interface.h>
#include <cstdio>

int main(int argc, char const** argv)
{
    CLuaBase lua_base;

    if (argc <= 1) {
        fprintf(stderr, "error: Need a script to run.\n");
        return 1;
    }

    switch (lua_base.load_and_run_file(argv[1])) {
    case CLuaBase::Success:
        return static_cast<int>(lua_base.CheckNumber(-1));
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
