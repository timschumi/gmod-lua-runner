#include "CLuaBase.h"
#include <cassert>
#include <chrono>
#include <cstring>
#include <lua.hpp>
#include <thread>

CLuaBase::CLuaBase()
{
    lua_state = luaL_newstate();
    lua_state->luabase = this;

    lua_pushvalue(lua_state, LUA_GLOBALSINDEX);
#define REGISTER_LUA_FUNCTION(name, impl)             \
    lua_pushstring(lua_state, name);                  \
    lua_pushcfunction(lua_state, lua$##impl##$entry); \
    lua_settable(lua_state, -3);
#define REGISTER_LUA_MODULE_START(name) \
    lua_pushstring(lua_state, name);    \
    lua_newtable(lua_state);
#define REGISTER_LUA_MODULE_END() \
    lua_settable(lua_state, -3);
    ENUMERATE_LUA_FUNCTIONS(REGISTER_LUA_FUNCTION, REGISTER_LUA_MODULE_START, REGISTER_LUA_MODULE_END)
#undef REGISTER_LUA_FUNCTION
#undef REGISTER_LUA_MODULE_START
#undef REGISTER_LUA_MODULE_END
    lua_pop(lua_state, 1);
}

CLuaBase::~CLuaBase()
{
    unload_modules();

    lua_close(lua_state);
}

CLuaBase::RunFileResult CLuaBase::load_and_run_file(char const* path)
{
    int ret = luaL_loadfile(lua_state, path);

    switch (ret) {
    case LUA_ERRSYNTAX:
        return RunFileResult::SyntaxError;
    case LUA_ERRMEM:
        return RunFileResult::MemoryAllocationError;
    case LUA_ERRFILE:
        return RunFileResult::FileError;
    default:
        break;
    }

    ret = lua_pcall(lua_state, 0, 1, 0);

    switch (ret) {
    case LUA_ERRRUN:
        return RunFileResult::RuntimeError;
    case LUA_ERRMEM:
        return RunFileResult::MemoryAllocationError;
    case LUA_ERRERR:
        return RunFileResult::ErrorError;
    default:
        break;
    }

    return RunFileResult::Success;
}

bool CLuaBase::is_active()
{
    for (auto coroutine : coroutines) {
        int coroutine_status = lua_status(coroutine);

        if (coroutine_status == LUA_YIELD)
            return true;
    }

    return false;
}

void CLuaBase::run_event_loop()
{
    while (is_active()) {
        // hook.Run("Think")
        lua_pushcfunction(lua_state, CLuaBase::lua$hook_run$entry);
        lua_pushstring(lua_state, "Think");
        lua_call(lua_state, 1, 0);

        // hook.Run("Tick")
        lua_pushcfunction(lua_state, CLuaBase::lua$hook_run$entry);
        lua_pushstring(lua_state, "Tick");
        lua_call(lua_state, 1, 0);

        // Run timers whose cooldowns have expired.
        for (auto& timer : timers) {
            if (timer.second.cooldown > 0)
                continue;

            timer.second.cooldown = timer.second.delay;
            lua_rawgeti(lua_state, LUA_REGISTRYINDEX, timer.second.function);
            lua_call(lua_state, 0, 0);
        }

        // Shift everything by 1/66th of a second.
        constexpr double delta = 1.0 / 66;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int64_t>(1000 * delta)));
        uptime += delta;
        for (auto& timer : timers)
            timer.second.cooldown -= delta;
    }
}

int CLuaBase::Top()
{
    return lua_gettop(lua_state);
}

void CLuaBase::Push(int iStackPos)
{
    lua_pushvalue(lua_state, iStackPos);
}

void CLuaBase::Pop(int iAmt)
{
    lua_pop(lua_state, iAmt);
}

void CLuaBase::GetTable(int iStackPos)
{
    lua_gettable(lua_state, iStackPos);
}

void CLuaBase::GetField(int iStackPos, char const* strName)
{
    lua_getfield(lua_state, iStackPos, strName);
}

void CLuaBase::SetField(int iStackPos, char const* strName)
{
    lua_setfield(lua_state, iStackPos, strName);
}

void CLuaBase::CreateTable()
{
    lua_newtable(lua_state);
}

void CLuaBase::SetTable(int iStackPos)
{
    lua_settable(lua_state, iStackPos);
}

void CLuaBase::SetMetaTable(int iStackPos)
{
    lua_setmetatable(lua_state, iStackPos);
}

bool CLuaBase::GetMetaTable(int i)
{
    return lua_getmetatable(lua_state, i);
}

void CLuaBase::Call(int iArgs, int iResults)
{
    lua_call(lua_state, iArgs, iResults);
}

int CLuaBase::PCall(int iArgs, int iResults, int iErrorFunc)
{
    return lua_pcall(lua_state, iArgs, iResults, iErrorFunc);
}

int CLuaBase::Equal(int iA, int iB)
{
    return lua_equal(lua_state, iA, iB);
}

int CLuaBase::RawEqual(int iA, int iB)
{
    return lua_rawequal(lua_state, iA, iB);
}

void CLuaBase::Insert(int iStackPos)
{
    lua_insert(lua_state, iStackPos);
}

void CLuaBase::Remove(int iStackPos)
{
    lua_remove(lua_state, iStackPos);
}

int CLuaBase::Next(int iStackPos)
{
    return lua_next(lua_state, iStackPos);
}

void* CLuaBase::NewUserdata(unsigned int iSize)
{
    return lua_newuserdata(lua_state, iSize);
}

void CLuaBase::ThrowError(char const* strError)
{
    lua_pushstring(lua_state, strError);
    lua_error(lua_state);
}

void CLuaBase::CheckType(int iStackPos, int iType)
{
    luaL_checktype(lua_state, iStackPos, iType);
}

void CLuaBase::ArgError(int iArgNum, char const* strMessage)
{
    luaL_argerror(lua_state, iArgNum, strMessage);
}

void CLuaBase::RawGet(int iStackPos)
{
    lua_rawget(lua_state, iStackPos);
}

void CLuaBase::RawSet(int iStackPos)
{
    lua_rawset(lua_state, iStackPos);
}

char const* CLuaBase::GetString(int iStackPos, unsigned int* iOutLen)
{
    size_t output_length = 0;
    char const* string = lua_tolstring(lua_state, iStackPos, &output_length);

    if (iOutLen) {
        assert(output_length <= UINT_MAX);
        *iOutLen = output_length;
    }

    return string;
}

double CLuaBase::GetNumber(int iStackPos)
{
    return lua_tonumber(lua_state, iStackPos);
}

bool CLuaBase::GetBool(int iStackPos)
{
    return lua_toboolean(lua_state, iStackPos);
}

GarrysMod::Lua::CFunc CLuaBase::GetCFunction(int iStackPos)
{
    return lua_tocfunction(lua_state, iStackPos);
}

void* CLuaBase::GetUserdata(int iStackPos)
{
    return lua_touserdata(lua_state, iStackPos);
}

void CLuaBase::PushNil()
{
    lua_pushnil(lua_state);
}

void CLuaBase::PushString(char const* val, unsigned int iLen)
{
    if (!iLen)
        iLen = strlen(val);

    lua_pushlstring(lua_state, val, iLen);
}

void CLuaBase::PushNumber(double val)
{
    lua_pushnumber(lua_state, val);
}

void CLuaBase::PushBool(bool val)
{
    lua_pushboolean(lua_state, val);
}

void CLuaBase::PushCFunction(GarrysMod::Lua::CFunc val)
{
    lua_pushcfunction(lua_state, val);
}

void CLuaBase::PushCClosure(GarrysMod::Lua::CFunc val, int iVars)
{
    lua_pushcclosure(lua_state, val, iVars);
}

void CLuaBase::PushUserdata(void* pVoid)
{
    lua_pushlightuserdata(lua_state, pVoid);
}

int CLuaBase::ReferenceCreate()
{
    return luaL_ref(lua_state, LUA_REGISTRYINDEX);
}

void CLuaBase::ReferenceFree(int i)
{
    luaL_unref(lua_state, LUA_REGISTRYINDEX, i);
}

void CLuaBase::ReferencePush(int i)
{
    lua_rawgeti(lua_state, LUA_REGISTRYINDEX, i);
}

void CLuaBase::PushSpecial(int iType)
{
    switch (iType) {
    case GarrysMod::Lua::SPECIAL_GLOB:
        lua_pushvalue(lua_state, LUA_GLOBALSINDEX);
        return;

    case GarrysMod::Lua::SPECIAL_ENV:
        lua_pushvalue(lua_state, LUA_ENVIRONINDEX);
        return;

    case GarrysMod::Lua::SPECIAL_REG:
        lua_pushvalue(lua_state, LUA_REGISTRYINDEX);
        return;

    default:
        assert(false);
    }
}

bool CLuaBase::IsType(int iStackPos, int iType)
{
    return lua_type(lua_state, iStackPos) == iType;
}

int CLuaBase::GetType(int iStackPos)
{
    return lua_type(lua_state, iStackPos);
}

char const* CLuaBase::GetTypeName(int iType)
{
    return lua_typename(lua_state, iType);
}

void CLuaBase::CreateMetaTableType(char const* strName, int iType)
{
    // FIXME: Implement CreateMetaTableType.
    assert(false);
}

char const* CLuaBase::CheckString(int iStackPos)
{
    return luaL_checkstring(lua_state, iStackPos);
}

double CLuaBase::CheckNumber(int iStackPos)
{
    return luaL_checknumber(lua_state, iStackPos);
}

int CLuaBase::ObjLen(int iStackPos)
{
    size_t length = lua_objlen(lua_state, iStackPos);
    assert(length <= INT_MAX && length >= INT_MIN);
    return static_cast<int>(length);
}

QAngle const& CLuaBase::GetAngle(int iStackPos)
{
    // FIXME: Implement GetAngle.
    assert(false);
}

Vector const& CLuaBase::GetVector(int iStackPos)
{
    // FIXME: Implement GetVector.
    assert(false);
}

void CLuaBase::PushAngle(QAngle const& val)
{
    // FIXME: Implement PushAngle.
    assert(false);
}

void CLuaBase::PushVector(Vector const& val)
{
    // FIXME: Implement PushVector.
    assert(false);
}

void CLuaBase::SetState(lua_State* L)
{
    lua_state = L;
}

int CLuaBase::CreateMetaTable(char const* strName)
{
    // FIXME: Implement CreateMetaTable.
    assert(false);
}

bool CLuaBase::PushMetaTable(int iType)
{
    // FIXME: Implement PushMetaTable.
    assert(false);
}

void CLuaBase::PushUserType(void* data, int iType)
{
    // FIXME: Implement PushUserType.
    assert(false);
}

void CLuaBase::SetUserType(int iStackPos, void* data)
{
    // FIXME: Implement SetUserType.
    assert(false);
}
