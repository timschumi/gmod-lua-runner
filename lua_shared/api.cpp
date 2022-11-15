#include <lua.hpp>

// Not sure why this exists, but apparently it's just an alias for the not-real function.
extern "C" int lua_resume_real(lua_State* L, int nargs)
{
    return lua_resume(L, nargs);
}
