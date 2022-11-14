#include <CLuaBase.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARGUMENT_LIMIT 3

static int stub_lua_function(lua_State* state)
{
    return 0;
}

static bool call_with_generated_arguments(CLuaBase& lua_base, size_t argument_limit, size_t pushed_arguments)
{
    pid_t child = fork();

    if (child < 0) {
        perror("fork failed");
        exit(1);
    }

    if (child == 0) {
        int fd = open("/dev/null", O_RDWR);
        dup2(fd, 1);
        dup2(fd, 2);

        lua_base.PCall(pushed_arguments, 0, 0);
        exit(0);
    }

    int child_status = 0;
    if (waitpid(child, &child_status, 0) < 0) {
        perror("waitpid failed");
        exit(1);
    }

    if (WIFEXITED(child_status) && WEXITSTATUS(child_status) != 0) {
        printf("child exited abnormally\n");
        return false;
    }

    if (WIFSIGNALED(child_status)) {
        return false;
    }

    if (pushed_arguments < argument_limit) {
        bool success;

        lua_base.PushNil();
        success = call_with_generated_arguments(lua_base, argument_limit, pushed_arguments + 1);
        lua_base.Pop(1);
        if (!success)
            return false;

        lua_base.PushNumber(1);
        success = call_with_generated_arguments(lua_base, argument_limit, pushed_arguments + 1);
        lua_base.Pop(1);
        if (!success)
            return false;

        lua_base.PushString("test", 0);
        success = call_with_generated_arguments(lua_base, argument_limit, pushed_arguments + 1);
        lua_base.Pop(1);
        if (!success)
            return false;

        lua_base.PushCFunction(stub_lua_function);
        success = call_with_generated_arguments(lua_base, argument_limit, pushed_arguments + 1);
        lua_base.Pop(1);
        if (!success)
            return false;

        lua_base.CreateTable();
        success = call_with_generated_arguments(lua_base, argument_limit, pushed_arguments + 1);
        lua_base.Pop(1);
        if (!success)
            return false;
    }

    return true;
}

int main(int argc, char const** argv)
{
    bool successful = true;
    CLuaBase lua_base;

    lua_base.PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

    // FIXME: Also fuzz functions from metatables.
#define MODULE_START(name) \
    lua_base.GetField(-1, name);
#define FUNCTION(name, impl)                                                       \
    lua_base.GetField(-1, name);                                                   \
    {                                                                              \
        bool success = call_with_generated_arguments(lua_base, ARGUMENT_LIMIT, 0); \
        if (!success) {                                                            \
            printf("%s failed\n", #impl);                                          \
            successful = false;                                                    \
        }                                                                          \
    }                                                                              \
    lua_base.Pop(1);
#define MODULE_END() \
    lua_base.Pop(1);
    ENUMERATE_LUA_FUNCTIONS(FUNCTION, MODULE_START, MODULE_END)
#undef MODULE_START
#undef FUNCTION
#undef MODULE_END

    lua_base.Pop(1);

    return successful ? 0 : 1;
}
