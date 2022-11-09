#pragma once

#include <GarrysMod/Lua/Interface.h>
#include <list>
#include <map>
#include <optional>
#include <string>

#define ENUMERATE_LUA_FUNCTIONS(FUNCTION, MODULE_START, MODULE_END) \
    FUNCTION("Color", Color)                                        \
    FUNCTION("CreateConVar", CreateConVar)                          \
    FUNCTION("CurTime", CurTime)                                    \
    FUNCTION("include", include)                                    \
    FUNCTION("print", print)                                        \
    FUNCTION("PrintTable", PrintTable)                              \
    FUNCTION("require", require)                                    \
    MODULE_START("coroutine")                                       \
    FUNCTION("create", coroutine_create)                            \
    FUNCTION("resume", coroutine_resume)                            \
    FUNCTION("running", coroutine_running)                          \
    FUNCTION("yield", coroutine_yield)                              \
    MODULE_END()                                                    \
    MODULE_START("hook")                                            \
    FUNCTION("Add", hook_Add)                                       \
    FUNCTION("Call", hook_Call)                                     \
    FUNCTION("GetTable", hook_GetTable)                             \
    FUNCTION("Remove", hook_Remove)                                 \
    FUNCTION("Run", hook_Run)                                       \
    MODULE_END()                                                    \
    MODULE_START("table")                                           \
    FUNCTION("Add", table_Add)                                      \
    MODULE_END()                                                    \
    MODULE_START("timer")                                           \
    FUNCTION("Create", timer_Create)                                \
    MODULE_END()

#define ENUMERATE_METATABLES(METATABLE_START, METATABLE_END, TABLE_START, TABLE_END, FUNCTION) \
    METATABLE_START("Color")                                                                   \
    FUNCTION("__eq", Color_eq)                                                                 \
    METATABLE_END()                                                                            \
    METATABLE_START("ConVar")                                                                  \
    TABLE_START("__index")                                                                     \
    FUNCTION("GetBool", ConVar_GetBool)                                                        \
    TABLE_END()                                                                                \
    METATABLE_END()

class CLuaBase : public GarrysMod::Lua::ILuaBase {
public:
    CLuaBase();
    ~CLuaBase();

    enum RunFileResult {
        Success,
        SyntaxError,
        MemoryAllocationError,
        FileError,
        RuntimeError,
        ErrorError,
    };

    RunFileResult load_and_run_file(char const* path);
    RunFileResult load_and_run_file_or_show_error(char const* path);
    bool is_active();
    void run_event_loop();

private:
    lua_State* lua_state { nullptr };

    double uptime { 0 };

    std::list<void*> loaded_module_handles;
    void unload_modules();

    std::map<std::string, std::map<std::string, int>> registered_hooks;

    std::list<lua_State*> coroutines;

    struct Timer {
        double delay;
        double repetitions;
        int function;
        double cooldown;
    };
    std::map<std::string, Timer> timers;

    // TODO: Maybe move this to libtier1.
    struct ConVar {
        std::string value;
        long flags;
        std::optional<std::string> helptext;
        std::optional<double> min;
        std::optional<double> max;
    };
    std::map<std::string, ConVar> convars;

    // Standard library implementation.
#define DECLARE_LUA_FUNCTION(name, impl)                     \
    static int lua$##impl##$entry(lua_State* state)          \
    {                                                        \
        auto base = dynamic_cast<CLuaBase*>(state->luabase); \
        base->SetState(state);                               \
        return base->lua$##impl();                           \
    }                                                        \
    int lua$##impl();
#define DECLARE_LUA_MODULE_START(name)
#define DECLARE_LUA_MODULE_END()
    ENUMERATE_LUA_FUNCTIONS(DECLARE_LUA_FUNCTION, DECLARE_LUA_MODULE_START, DECLARE_LUA_MODULE_END)
#undef DECLARE_LUA_FUNCTION
#undef DECLARE_LUA_MODULE_START
#undef DECLARE_LUA_MODULE_END

    // Metatables.
#define NOOP0()
#define NOOP1(unused)
#define DECLARE_METATABLE_FUNCTION(name, impl)               \
    static int lua$meta$##impl##$entry(lua_State* state)     \
    {                                                        \
        auto base = dynamic_cast<CLuaBase*>(state->luabase); \
        base->SetState(state);                               \
        return base->lua$meta$##impl();                      \
    }                                                        \
    int lua$meta$##impl();
    ENUMERATE_METATABLES(NOOP1, NOOP0, NOOP1, NOOP0, DECLARE_METATABLE_FUNCTION)
#undef NOOP0
#undef NOOP1

public:
    // Virtual functions from ILuaBase.
    int Top() override;
    void Push(int iStackPos) override;
    void Pop(int iAmt) override;
    void GetTable(int iStackPos) override;
    void GetField(int iStackPos, char const* strName) override;
    void SetField(int iStackPos, char const* strName) override;
    void CreateTable() override;
    void SetTable(int iStackPos) override;
    void SetMetaTable(int iStackPos) override;
    bool GetMetaTable(int i) override;
    void Call(int iArgs, int iResults) override;
    int PCall(int iArgs, int iResults, int iErrorFunc) override;
    int Equal(int iA, int iB) override;
    int RawEqual(int iA, int iB) override;
    void Insert(int iStackPos) override;
    void Remove(int iStackPos) override;
    int Next(int iStackPos) override;
    void* NewUserdata(unsigned int iSize) override;
    void ThrowError(char const* strError) override;
    void CheckType(int iStackPos, int iType) override;
    void ArgError(int iArgNum, char const* strMessage) override;
    void RawGet(int iStackPos) override;
    void RawSet(int iStackPos) override;
    char const* GetString(int iStackPos, unsigned int* iOutLen) override;
    double GetNumber(int iStackPos) override;
    bool GetBool(int iStackPos) override;
    GarrysMod::Lua::CFunc GetCFunction(int iStackPos) override;
    void* GetUserdata(int iStackPos) override;
    void PushNil() override;
    void PushString(char const* val, unsigned int iLen) override;
    void PushNumber(double val) override;
    void PushBool(bool val) override;
    void PushCFunction(GarrysMod::Lua::CFunc val) override;
    void PushCClosure(GarrysMod::Lua::CFunc val, int iVars) override;
    void PushUserdata(void* pVoid) override;
    int ReferenceCreate() override;
    void ReferenceFree(int i) override;
    void ReferencePush(int i) override;
    void PushSpecial(int iType) override;
    bool IsType(int iStackPos, int iType) override;
    int GetType(int iStackPos) override;
    char const* GetTypeName(int iType) override;
    void CreateMetaTableType(char const* strName, int iType) override;
    char const* CheckString(int iStackPos) override;
    double CheckNumber(int iStackPos) override;
    int ObjLen(int iStackPos) override;
    QAngle const& GetAngle(int iStackPos) override;
    Vector const& GetVector(int iStackPos) override;
    void PushAngle(QAngle const& val) override;
    void PushVector(Vector const& val) override;
    void SetState(lua_State* L) override;
    int CreateMetaTable(char const* strName) override;
    bool PushMetaTable(int iType) override;
    void PushUserType(void* data, int iType) override;
    void SetUserType(int iStackPos, void* data) override;
};
