#pragma once

#include <GarrysMod/Lua/Interface.h>
#include <list>
#include <map>
#include <string>

#define ENUMERATE_LUA_FUNCTIONS(FUNCTION, MODULE_START, MODULE_END) \
    FUNCTION("print", print)                                        \
    FUNCTION("PrintTable", print_table)                             \
    FUNCTION("require", require)                                    \
    MODULE_START("coroutine")                                       \
    FUNCTION("create", coroutine_create)                            \
    FUNCTION("resume", coroutine_resume)                            \
    FUNCTION("running", coroutine_running)                          \
    FUNCTION("yield", coroutine_yield)                              \
    MODULE_END()                                                    \
    MODULE_START("hook")                                            \
    FUNCTION("Add", hook_add)                                       \
    FUNCTION("Call", hook_call)                                     \
    FUNCTION("GetTable", hook_get_table)                            \
    FUNCTION("Remove", hook_remove)                                 \
    FUNCTION("Run", hook_run)                                       \
    MODULE_END()                                                    \
    MODULE_START("timer")                                           \
    FUNCTION("Create", timer_create)                                \
    MODULE_END()

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
    bool is_active();
    void run_event_loop();

private:
    lua_State* lua_state { nullptr };

    std::list<void*> loaded_module_handles;
    void unload_modules();

    std::map<std::string, std::map<std::string, int>> registered_hooks;

    std::list<lua_State*> coroutines;

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
