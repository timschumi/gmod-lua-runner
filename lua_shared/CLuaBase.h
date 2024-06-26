#pragma once

#include <GarrysMod/Lua/Interface.h>
#include <chrono>
#include <filesystem>
#include <functional>
#include <list>
#include <map>
#include <optional>
#include <string>

#define ENUMERATE_LUA_FUNCTIONS(FUNCTION, MODULE_START, MODULE_END) \
    FUNCTION("Color", Color)                                        \
    FUNCTION("CreateConVar", CreateConVar)                          \
    FUNCTION("CurTime", CurTime)                                    \
    FUNCTION("ErrorNoHaltWithStack", ErrorNoHaltWithStack)          \
    FUNCTION("GetConVar", GetConVar)                                \
    FUNCTION("include", include)                                    \
    FUNCTION("isfunction", isfunction)                              \
    FUNCTION("istable", istable)                                    \
    FUNCTION("MsgC", MsgC)                                          \
    FUNCTION("PrintTable", PrintTable)                              \
    FUNCTION("require", require)                                    \
    FUNCTION("SysTime", SysTime)                                    \
    FUNCTION("type", type)                                          \
    MODULE_START("concommand")                                      \
    FUNCTION("Add", concommand_Add)                                 \
    MODULE_END()                                                    \
    MODULE_START("file")                                            \
    FUNCTION("Find", file_Find)                                     \
    FUNCTION("Open", file_Open)                                     \
    MODULE_END()                                                    \
    MODULE_START("hook")                                            \
    FUNCTION("Add", hook_Add)                                       \
    FUNCTION("Call", hook_Call)                                     \
    FUNCTION("GetTable", hook_GetTable)                             \
    FUNCTION("Remove", hook_Remove)                                 \
    FUNCTION("Run", hook_Run)                                       \
    MODULE_END()                                                    \
    MODULE_START("string")                                          \
    FUNCTION("EndsWith", string_EndsWith)                           \
    FUNCTION("Explode", string_Explode)                             \
    FUNCTION("Right", string_Right)                                 \
    FUNCTION("Split", string_Split)                                 \
    FUNCTION("StartWith", string_StartWith)                         \
    MODULE_END()                                                    \
    MODULE_START("table")                                           \
    FUNCTION("Add", table_Add)                                      \
    FUNCTION("Copy", table_Copy)                                    \
    FUNCTION("Count", table_Count)                                  \
    FUNCTION("Inherit", table_Inherit)                              \
    MODULE_END()                                                    \
    MODULE_START("timer")                                           \
    FUNCTION("Create", timer_Create)                                \
    FUNCTION("Exists", timer_Exists)                                \
    FUNCTION("Remove", timer_Remove)                                \
    MODULE_END()

#define ENUMERATE_METATABLES(METATABLE_START, METATABLE_END, TABLE_START, TABLE_END, FUNCTION) \
    METATABLE_START("Color")                                                                   \
    FUNCTION("__eq", Color_eq)                                                                 \
    METATABLE_END()                                                                            \
    METATABLE_START("ConVar")                                                                  \
    TABLE_START("__index")                                                                     \
    FUNCTION("GetBool", ConVar_GetBool)                                                        \
    FUNCTION("SetBool", ConVar_SetBool)                                                        \
    TABLE_END()                                                                                \
    METATABLE_END()                                                                            \
    METATABLE_START("File")                                                                    \
    TABLE_START("__index")                                                                     \
    FUNCTION("Close", File_Close)                                                              \
    FUNCTION("Read", File_Read)                                                                \
    FUNCTION("Size", File_Size)                                                                \
    TABLE_END()                                                                                \
    METATABLE_END()

// https://wiki.facepunch.com/gmod/Enums/FCVAR
#define ENUMERATE_CONVAR_FLAGS(E)           \
    E(FCVAR_NONE, 0)                        \
    E(FCVAR_UNREGISTERED, 1)                \
    E(FCVAR_GAMEDLL, 4)                     \
    E(FCVAR_CLIENTDLL, 8)                   \
    E(FCVAR_PROTECTED, 32)                  \
    E(FCVAR_SPONLY, 64)                     \
    E(FCVAR_ARCHIVE, 128)                   \
    E(FCVAR_NOTIFY, 256)                    \
    E(FCVAR_USERINFO, 512)                  \
    E(FCVAR_PRINTABLEONLY, 1024)            \
    E(FCVAR_UNLOGGED, 2048)                 \
    E(FCVAR_NEVER_AS_STRING, 4096)          \
    E(FCVAR_REPLICATED, 8192)               \
    E(FCVAR_CHEAT, 16384)                   \
    E(FCVAR_DEMO, 65536)                    \
    E(FCVAR_DONTRECORD, 131072)             \
    E(FCVAR_LUA_CLIENT, 262144)             \
    E(FCVAR_LUA_SERVER, 524288)             \
    E(FCVAR_NOT_CONNECTED, 4194304)         \
    E(FCVAR_ARCHIVE_XBOX, 16777216)         \
    E(FCVAR_SERVER_CAN_EXECUTE, 268435456)  \
    E(FCVAR_SERVER_CANNOT_QUERY, 536870912) \
    E(FCVAR_CLIENTCMD_CAN_EXECUTE, 1073741824)

class CLuaBase : public GarrysMod::Lua::ILuaBase {
public:
    CLuaBase();
    ~CLuaBase();

    void set_base_directory(std::filesystem::path const&);

    int load_file(char const* path);
    static int print_error_with_stack_trace(lua_State* state);
    static int print_stack_trace(lua_State* state);

    bool is_active();
    std::optional<std::function<std::optional<bool>(GarrysMod::Lua::ILuaBase*)>> is_active_override;

    void run_event_loop();

private:
    lua_State* main_lua_state { nullptr };

    std::filesystem::path base_directory;

    double uptime { 0 };
    std::chrono::time_point<std::chrono::system_clock> boot_system_clock;

    std::map<std::string, void*> loaded_module_handles;
    void unload_modules();

    std::map<std::string, std::map<std::string, int>> registered_hooks;

    struct Timer {
        double delay;
        std::optional<double> repetitions;
        int function;
        double cooldown;
    };
    std::map<std::string, Timer> timers;

    // TODO: Maybe move this to libtier1.
    enum ConVarFlags {
#define DEFINE_ENUM_ENTRY(name, value) name = value,
        ENUMERATE_CONVAR_FLAGS(DEFINE_ENUM_ENTRY)
#undef DEFINE_ENUM_ENTRY
    };
    struct ConVar {
        std::string value;
        long flags;
        std::optional<std::string> helptext;
        std::optional<double> min;
        std::optional<double> max;
    };
    std::map<std::string, ConVar> convars;

    struct ConCommand {
        int callback;
        std::string help_text;
        int flags;
    };
    std::map<std::string, ConCommand> concommands;

    // Standard library implementation.
#define DECLARE_LUA_FUNCTION(name, impl)                     \
    int lua$##impl##$original { -1 };                        \
    static int lua$##impl##$entry(lua_State* state)          \
    {                                                        \
        auto base = dynamic_cast<CLuaBase*>(state->luabase); \
        return base->lua$##impl(state);                      \
    }                                                        \
    int lua$##impl(lua_State* state);
#define DECLARE_LUA_MODULE_START(name)
#define DECLARE_LUA_MODULE_END()
    ENUMERATE_LUA_FUNCTIONS(DECLARE_LUA_FUNCTION, DECLARE_LUA_MODULE_START, DECLARE_LUA_MODULE_END)
#undef DECLARE_LUA_FUNCTION
#undef DECLARE_LUA_MODULE_START
#undef DECLARE_LUA_MODULE_END

    // Not really a full implementation, just a shim that delegates to the existing implementation.
    static int lua$tostring$entry(lua_State* state);

    // Metatables.
#define NOOP0()
#define NOOP1(unused)
#define DECLARE_METATABLE_FUNCTION(name, impl)               \
    static int lua$meta$##impl##$entry(lua_State* state)     \
    {                                                        \
        auto base = dynamic_cast<CLuaBase*>(state->luabase); \
        return base->lua$meta$##impl(state);                 \
    }                                                        \
    int lua$meta$##impl(lua_State* state);
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
