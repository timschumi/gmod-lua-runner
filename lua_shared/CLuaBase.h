#pragma once

#include <GarrysMod/Lua/LuaBase.h>

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

    RunFileResult RunFile(char const* path);

private:
    lua_State* lua_state { nullptr };

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
