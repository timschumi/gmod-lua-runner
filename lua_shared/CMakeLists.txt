set(SOURCES
    CLuaBase.cpp
    libraries/base.cpp
    libraries/base_color.cpp
    libraries/base_convars.cpp
    libraries/coroutine.cpp
    libraries/debug.cpp
    libraries/file.cpp
    libraries/hook.cpp
    libraries/string.cpp
    libraries/table.cpp
    libraries/timer.cpp
)

add_library(lua_shared SHARED ${SOURCES})
set_target_properties(lua_shared PROPERTIES PREFIX "")
target_link_libraries(lua_shared PUBLIC gmod-module-base)
target_link_libraries(lua_shared PRIVATE luajit tier0 ${CMAKE_DL_LIBS})
target_include_directories(lua_shared PUBLIC ./)
install(TARGETS lua_shared DESTINATION ./${GMOD_LUASHARED_DIR})
file(RELATIVE_PATH luashared_to_libtier0 "/${GMOD_LUASHARED_DIR}" "/${GMOD_LIBTIER0_DIR}")
set_target_properties(lua_shared PROPERTIES INSTALL_RPATH "$ORIGIN/${luashared_to_libtier0}")
target_compile_definitions(lua_shared PRIVATE "-DGMOD_MODULE_ARCH=\"${GMOD_MODULE_ARCH}\"")
