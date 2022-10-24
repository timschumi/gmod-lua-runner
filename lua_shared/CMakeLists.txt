set(SOURCES
    CLuaBase.cpp
    libraries/base.cpp
    libraries/hook.cpp
    libraries/timer.cpp
)

add_library(lua_shared SHARED ${SOURCES})
set_target_properties(lua_shared PROPERTIES PREFIX "")
target_link_libraries(lua_shared PUBLIC gmod-module-base)
target_link_libraries(lua_shared PRIVATE luajit tier0)
target_include_directories(lua_shared PUBLIC ./)
